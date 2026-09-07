#include <wetman/utils/net/client/unix_client.h>

#include <wetman/utils/net/message.h>
#include <wetman/utils/net/return_code.h>
#include <wetman/utils/type.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

typedef struct {
    i32 fdUnixSocket;
} __UnitClientContext;

Message __UnixClient_RequestHandler(
        Client*  client,
        Message* requestMessage,
        Arena*   arena)
{
    Message responseMessage = Message_New();
    __UnitClientContext* context = (__UnitClientContext*)client->__context;

    DataStream_WriteMsg(&requestMessage->bodyStream, context->fdUnixSocket, &requestMessage->fdStream);
    if (requestMessage->bodyStream.lastResult != DATA_STREAM_RESULT_SUCCESS) {
        // TODO: proper failure handling
        perror("Failed to write request");
        // close(context->fdUnixSocket);
        responseMessage.bodyStream.lastResult = DATA_STREAM_RESULT_FAILED_WRITE;
        return responseMessage;
    }

    DataStream responseHeaderData = DataStream_ReadMsg(
            context->fdUnixSocket, arena, RESPONSE_HEADER_SERIALIZED_LEN, &responseMessage.fdStream);

    // Since DataStream does not own the data and holds only a data slice
    // no deep copy is performed here
    DataStream responseHeaderDataCopy = responseHeaderData;
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseHeaderDataCopy);

    DataStream responseBodyData = DataStream_ReadMsg(
            context->fdUnixSocket, arena, responseHeader.msgLen, &responseMessage.fdStream);

    DataStream_Append(&responseMessage.bodyStream, &responseHeaderData, arena);
    DataStream_Append(&responseMessage.bodyStream, &responseBodyData, arena);

    return responseMessage;
}

void __UnixClient_DisconnectHandler(Client* client)
{
    __UnitClientContext* context = (__UnitClientContext*)client->__context;
    close(context->fdUnixSocket);
    Arena_Free(&client->__arena);
}

Client UnixClient_Connect(const char* socketPath)
{
    Arena arena = Arena_WithPageCapacity(sizeof(__UnitClientContext));
    __UnitClientContext* context = (__UnitClientContext*)Arena_Alloc(
            &arena, sizeof(__UnitClientContext));

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath, sizeof(addr.sun_path) - 1);

    const i32 fdUnixSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fdUnixSocket < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    if (connect(fdUnixSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Failed to connect to server");
        exit(1);
    }

    context->fdUnixSocket = fdUnixSocket;

    Client client = {
        .__arena          = arena,
        .__requestHandler = __UnixClient_RequestHandler,
        .__context        = (void*)context,
        .disconnect       = __UnixClient_DisconnectHandler,
    };

    return client;
}

