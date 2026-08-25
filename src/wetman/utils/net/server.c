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
#include <poll.h>
#include <fcntl.h>


#define __SERVER_MAX_CONNECTION_QUEUE_LEN 16
#define __SERVER_MAX_CLIENTS 64


int __Server_CreateSocket(const char* socketPath)
{
    struct sockaddr_un addr;
    const usize maxSocketPathLen = sizeof(addr.sun_path) - 1;

    if (strlen(socketPath) > maxSocketPathLen) {
        perror("UNIX socket path exceeds max allowed length\n");
        exit(1);
    }

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


typedef enum {
    CONN_STATE_FREE,
    CONN_STATE_READ_HEADER,
    CONN_STATE_READ_BODY,
    CONN_STATE_WRITE,
} __ConnState;

typedef struct {
    int         fd;         // client socket fd, -1 when slot is unused
    __ConnState state;      // current position in the read/write state machine

    // Read accumulation
    //   headerBuf: fixed inline buffer for the fixed-size request header
    //   bodyBuf:   arena-allocated after header is parsed (size from reqHeader.msgLen)
    //   readLen:   bytes accumulated so far in the current read phase
    //   readTarget: bytes needed to complete the current read phase
    char        headerBuf[REQUEST_HEADER_SERIALIZED_LEN];
    char*       bodyBuf;
    usize       readLen;
    usize       readTarget;

    RequestHeader reqHeader; // parsed from headerBuf once header read completes

    // Response writing
    //   response:     serialized response DataStream (header + body), populated
    //                 by EndpointRegistry_CallEndpoint after request is fully read
    //   writeOffset:  bytes of response already written to the client socket
    DataStream  response;
    usize       writeOffset;

    Arena       arena;      // per-request arena, freed on disconnect
} __ClientConn;

typedef struct {
    EventLoop*        eventLoop;
    int               fdServer;
    EndpointRegistry* endpointRegistry;
    __ClientConn      conns[__SERVER_MAX_CLIENTS];
} __ServerMainContext;


void __Server_DisconnectConn(__ClientConn* conn)
{
    if (conn->fd >= 0) {
        close(conn->fd);
        conn->fd = -1;
    }
    if (Arena_IsValid(&conn->arena)) {
        Arena_Free(&conn->arena);
    }
    conn->state = CONN_STATE_FREE;
    printf("Client disconnected\n");
}


void __Server_AcceptClient(__ServerMainContext* ctx)
{
    int fdClient = accept(ctx->fdServer, NULL, NULL);
    if (fdClient < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return;
        perror("Failed to accept client connection\n");
        return;
    }

    int flags = fcntl(fdClient, F_GETFL, 0);
    if (flags < 0 || fcntl(fdClient, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("Failed to set client socket non-blocking\n");
        close(fdClient);
        return;
    }

    for (int i = 0; i < __SERVER_MAX_CLIENTS; i++) {
        if (ctx->conns[i].state == CONN_STATE_FREE) {
            __ClientConn* conn = &ctx->conns[i];
            conn->fd         = fdClient;
            conn->state      = CONN_STATE_READ_HEADER;
            conn->bodyBuf    = NULL;
            conn->readLen    = 0;
            conn->readTarget = REQUEST_HEADER_SERIALIZED_LEN;
            conn->writeOffset = 0;
            conn->arena      = Arena_New();
            printf("Client connected (slot %d)\n", i);
            return;
        }
    }

    fprintf(stderr, "Max clients reached, rejecting connection\n");
    close(fdClient);
}


void __Server_ProcessRequest(__ClientConn* conn, EndpointRegistry* endpointRegistry)
{
    DataSlice bodyData;
    if (conn->bodyBuf) {
        bodyData = (DataSlice){
            .data = conn->bodyBuf,
            .len  = conn->reqHeader.msgLen,
        };
    } else {
        bodyData = (DataSlice){ .data = "", .len = 0 };
    }
    DataStream requestData = DataStream_WithData(bodyData);

    printf("Calling endpoint %d\n", conn->reqHeader.endpointId);

    conn->response = EndpointRegistry_CallEndpoint(
            endpointRegistry,
            conn->reqHeader.endpointId,
            &conn->arena,
            &requestData);

    conn->writeOffset = 0;
    conn->state = CONN_STATE_WRITE;
}


void __Server_HandleReadable(__ClientConn* conn, EndpointRegistry* endpointRegistry)
{
    if (conn->state == CONN_STATE_READ_HEADER) {
        while (conn->readLen < conn->readTarget) {
            isize n = read(conn->fd,
                    conn->headerBuf + conn->readLen,
                    conn->readTarget - conn->readLen);
            if (n > 0) {
                conn->readLen += (usize)n;
                continue;
            }
            if (n == 0) {
                __Server_DisconnectConn(conn);
                return;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) return;
            __Server_DisconnectConn(conn);
            return;
        }

        DataSlice headerData = {
            .data = conn->headerBuf,
            .len  = REQUEST_HEADER_SERIALIZED_LEN,
        };
        DataStream ds = DataStream_WithData(headerData);
        conn->reqHeader = RequestHeader_Deserialize(&ds);

        if (ds.lastResult != DATA_STREAM_RESULT_SUCCESS) {
            fprintf(stderr, "Failed to parse request header\n");
            __Server_DisconnectConn(conn);
            return;
        }

        printf("Read request header: endpoint=%d bodyLen=%u\n",
                conn->reqHeader.endpointId, conn->reqHeader.msgLen);

        if (conn->reqHeader.msgLen == 0) {
            __Server_ProcessRequest(conn, endpointRegistry);
            return;
        }

        conn->readLen = 0;
        conn->readTarget = conn->reqHeader.msgLen;

        conn->bodyBuf = Arena_Alloc(&conn->arena, conn->reqHeader.msgLen);
        if (!conn->bodyBuf) {
            fprintf(stderr, "Failed to allocate body buffer\n");
            __Server_DisconnectConn(conn);
            return;
        }

        conn->state = CONN_STATE_READ_BODY;
        return;
    }

    if (conn->state == CONN_STATE_READ_BODY) {
        while (conn->readLen < conn->readTarget) {
            isize n = read(conn->fd,
                    conn->bodyBuf + conn->readLen,
                    conn->readTarget - conn->readLen);
            if (n > 0) {
                conn->readLen += (usize)n;
                continue;
            }
            if (n == 0) {
                __Server_DisconnectConn(conn);
                return;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) return;
            __Server_DisconnectConn(conn);
            return;
        }

        printf("Read request body: %u bytes\n", conn->reqHeader.msgLen);

        __Server_ProcessRequest(conn, endpointRegistry);
        return;
    }
}


void __Server_HandleWritable(__ClientConn* conn)
{
    const char* data    = conn->response.__data.data + conn->writeOffset;
    usize       remaining = conn->response.__data.len - conn->writeOffset;

    isize n = write(conn->fd, data, remaining);
    if (n > 0) {
        conn->writeOffset += (usize)n;
        if (conn->writeOffset >= conn->response.__data.len) {
            printf("Response fully sent\n");
            __Server_DisconnectConn(conn);
        }
    } else if (n == 0) {
        __Server_DisconnectConn(conn);
    } else {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return;
        __Server_DisconnectConn(conn);
    }
}


void __Server_MainIter(Callback* callbackMeta)
{
    __ServerMainContext* ctx = (__ServerMainContext*)callbackMeta->payload;

    struct pollfd pfds[1 + __SERVER_MAX_CLIENTS];
    int           connIndices[1 + __SERVER_MAX_CLIENTS];
    int           nfds = 0;

    pfds[nfds].fd     = ctx->fdServer;
    pfds[nfds].events = POLLIN;
    pfds[nfds].revents = 0;
    connIndices[nfds] = -1;
    nfds++;

    for (int i = 0; i < __SERVER_MAX_CLIENTS; i++) {
        if (ctx->conns[i].state == CONN_STATE_FREE) continue;

        pfds[nfds].fd     = ctx->conns[i].fd;
        pfds[nfds].events = (ctx->conns[i].state == CONN_STATE_WRITE)
                ? (short)POLLOUT : (short)POLLIN;
        pfds[nfds].revents = 0;
        connIndices[nfds] = i;
        nfds++;
    }

    int ready = poll(pfds, (nfds_t)nfds, -1);
    if (ready < 0) {
        if (errno == EINTR) {
            Callback nextIter = Callback_New(
                    __Server_MainIter,
                    (void*)ctx,
                    callbackMeta->arena);
            nextIter.arenaPolicy = CALLBACK_ARENA_POLICY_DO_NOTHING;
            EventLoop_Push(ctx->eventLoop, nextIter);
            return;
        }
        perror("poll failed\n");
        return;
    }

    for (int i = 0; i < nfds; i++) {
        if (pfds[i].revents == 0) continue;

        if (connIndices[i] == -1) {
            if (pfds[i].revents & POLLIN) {
                __Server_AcceptClient(ctx);
            }
            continue;
        }

        __ClientConn* conn = &ctx->conns[connIndices[i]];

        if (pfds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
            __Server_DisconnectConn(conn);
            continue;
        }

        if (conn->state == CONN_STATE_WRITE) {
            if (pfds[i].revents & POLLOUT) {
                __Server_HandleWritable(conn);
            }
        } else if (conn->state == CONN_STATE_READ_HEADER
                || conn->state == CONN_STATE_READ_BODY) {
            if (pfds[i].revents & POLLIN) {
                __Server_HandleReadable(conn, ctx->endpointRegistry);
            }
        }
    }

    Callback nextIter = Callback_New(
            __Server_MainIter,
            (void*)ctx,
            callbackMeta->arena);
    nextIter.arenaPolicy = CALLBACK_ARENA_POLICY_DO_NOTHING;
    EventLoop_Push(ctx->eventLoop, nextIter);
}


int Server_Run(const char* socketPath, EndpointRegistry* endpointRegistry)
{
    int       fdServer = __Server_CreateSocket(socketPath);
    Arena     loopArena = Arena_New();
    EventLoop mainLoop = EventLoop_New();

    __ServerMainContext mainContext = {
        .eventLoop        = &mainLoop,
        .fdServer         = fdServer,
        .endpointRegistry = endpointRegistry,
    };
    memset(mainContext.conns, 0, sizeof(mainContext.conns));

    for (int i = 0; i < __SERVER_MAX_CLIENTS; i++) {
        mainContext.conns[i].fd = -1;
    }

    Callback nextIter = Callback_New(
            __Server_MainIter,
            (void*)(&mainContext),
            loopArena);
    nextIter.arenaPolicy = CALLBACK_ARENA_POLICY_DO_NOTHING;

    EventLoop_Push(&mainLoop, nextIter);
    EventLoop_Exec(&mainLoop);

    for (int i = 0; i < __SERVER_MAX_CLIENTS; i++) {
        if (mainContext.conns[i].state != CONN_STATE_FREE) {
            __Server_DisconnectConn(&mainContext.conns[i]);
        }
    }

    Arena_Free(&loopArena);
    close(fdServer);
    unlink(socketPath);

    return 0;
}
