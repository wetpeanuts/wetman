#ifndef WETMAN_UTILS_NET_CLIENT_CLIENT_H
#define WETMAN_UTILS_NET_CLIENT_CLIENT_H

#include <wetman/utils/data_stream.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/endpoint.h>
#include <wetman/utils/net/message.h>
#include <wetman/utils/net/return_code.h>


typedef struct Client Client;

typedef Message (*ClientRequestHandler)(
        Client*  client,
        Message* requestMessage,
        Arena*   arena);

typedef void(*ClientDisconnectHandler)(Client* client);

struct Client {
    Arena                   __arena;
    ClientRequestHandler    __requestHandler;
    void*                   __context;

    ClientDisconnectHandler disconnect;
};

ReturnCode Client_CallEndpoint(
        Client*  client,
        EndpointId endpointId,
        Arena*   arena,
        Message* requestMessage,
        Message* responseMessage);

#endif // WETMAN_UTILS_NET_CLIENT_CLIENT_H
