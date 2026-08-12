#include <wetman/utils/net/server.h>

#include <wetman/utils/async/callback.h>
#include <wetman/utils/async/event_loop.h>
#include <wetman/utils/data_stream.h>
#include <wetman/utils/macro.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/message.h>
#include <wetman/utils/type.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>


#define __SERVER_MAX_CONNECTION_QUEUE_LEN 16


int __Server_CreateSocket(const char* socketPath)
{
    struct sockaddr_un addr;
    const usize maxSocketPathLen = sizeof(addr.sun_path) - 1;

    if (strlen(socketPath) > maxSocketPathLen) {
        perror("UNIX socket path exceeds max allowed length\n");
        exit(1);
    }

    // Remove old socket file
    unlink(socketPath);

    int fdServer = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fdServer < 0) {
        perror("Failed to creare socket\n");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath, maxSocketPathLen);

    if (bind(fdServer, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Failed to bind socket\n");
        exit(1);
    }

    if (listen(fdServer, __SERVER_MAX_CONNECTION_QUEUE_LEN) < 0) {
        perror("Failed to start listening on socket\n");
        exit(1);
    }

    printf("Listening on %s\n", socketPath);

    return fdServer;
}

void __Server_WriteEmptyResponse(
        u32    fdClient,
        i32    returnCode,
        Arena* arena)
{
    ResponseHeader responseHeader = {
        .returnCode = returnCode,
        .msgLen     = 0,
    };

    DataStream dsResponse = DataStream_New();
    ResponseHeader_Serialize(&responseHeader, &dsResponse, arena);

    DataStream_Write(&dsResponse, fdClient);

    close(fdClient);
    printf("Client disconnected\n");
    return;
}

typedef struct {
    EventLoop*        eventLoop;
    Arena*            arena;
    EndpointRegistry* endpointRegistry;
    int               fdServer;
} __ServerMainContext;

void __Server_Main(__ServerMainContext* mainContext)
{
    int fdClient = accept(mainContext->fdServer, NULL, NULL);

    if (fdClient < 0) {
        perror("Failed to accept client connection\n");
        return;
    }

    printf("Client connected\n");

    // Process request header
    DataStream dsRequest = DataStream_Read(fdClient, mainContext->arena, REQUEST_HEADER_SERIALIZED_LEN);
    printf("Read request header: %lu bytes\n", dsRequest.__data.len);

    RequestHeader requestHeader = RequestHeader_Deserialize(&dsRequest);
    if (dsRequest.lastResult != DATA_STREAM_RESULT_SUCCESS) {
        fprintf(stderr, "Failed to parse data stream. Last error: %d\n", dsRequest.lastResult);
        __Server_WriteEmptyResponse(
                fdClient,
                RETURN_CODE_FAILED_TO_PARSE_REQUEST,
                mainContext->arena);
        return;
    }

    // Process request body
    dsRequest = DataStream_Read(fdClient, mainContext->arena, requestHeader.msgLen);
    printf("Read request body: %lu bytes\n", dsRequest.__data.len);

    if (dsRequest.lastResult != DATA_STREAM_RESULT_SUCCESS) {
        fprintf(stderr, "Failed to parse data stream. Last error: %d\n", dsRequest.lastResult);
        __Server_WriteEmptyResponse(
                fdClient,
                RETURN_CODE_FAILED_TO_PARSE_REQUEST,
                mainContext->arena);
        return;
    }

    printf("Calling endpoint %d\n", requestHeader.endpointId);

    DataStream dsResponse = EndpointRegistry_CallEndpoint(
            mainContext->endpointRegistry,
            requestHeader.endpointId,
            mainContext->arena,
            &dsRequest);

    DataStream_Write(&dsResponse, fdClient);

    close(fdClient);
    printf("Client disconnected\n");
}

void __Server_MainIter(Callback* callbackMeta)
{
    __ServerMainContext* mainContext = (__ServerMainContext*)callbackMeta->payload;

    __Server_Main(mainContext);

    Callback nextIter = Callback_New(
            __Server_MainIter,
            (void*)mainContext,
            callbackMeta->arena);
    nextIter.arenaPolicy = CALLBACK_ARENA_POLICY_RESET;

    EventLoop_Push(mainContext->eventLoop, nextIter);
}

int Server_Run(const char* socketPath, EndpointRegistry* endpointRegistry)
{
    int       fdServer  = __Server_CreateSocket(socketPath);
    Arena     mainArena = Arena_New();
    EventLoop mainLoop  = EventLoop_New();

    __ServerMainContext mainContext = {
        .eventLoop        = &mainLoop,
        .arena            = &mainArena,
        .endpointRegistry = endpointRegistry,
        .fdServer         = fdServer,
    };

    Callback nextIter = Callback_New(
            __Server_MainIter,
            (void*)(&mainContext),
            mainArena);
    nextIter.arenaPolicy = CALLBACK_ARENA_POLICY_RESET;

    EventLoop_Push(&mainLoop, nextIter);
    EventLoop_Exec(&mainLoop);

    Arena_Free(&mainArena);
    close(fdServer);
    unlink(socketPath);

    return 0;
}

