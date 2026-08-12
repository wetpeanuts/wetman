#ifndef WETMAN_UTILS_NET_CLIENT_H
#define WETMAN_UTILS_NET_CLIENT_H

#include <wetman/utils/data_stream.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/endpoint.h>
#include <wetman/utils/net/return_code.h>


typedef struct Client Client;

typedef DataStream (*ClientTransportRequest)(
        Client*     client,
        DataStream* requestData,
        Arena*      arena);

typedef struct {
    const char* socketPath;
} ClientUnixTransportContext;

struct Client {
    ClientTransportRequest     request;
    ClientUnixTransportContext unixContext;
};

Client Client_New(const char* socketPath);
DataStream ClientTransport_UnixRequest(
        Client*     client,
        DataStream* requestData,
        Arena*      arena);
ReturnCode Client_CallEndpoint(
        Client*     client,
        EndpointId  endpointId,
        Arena*      arena,
        DataStream* requestBody,
        DataStream* responseData);

#endif // WETMAN_UTILS_NET_CLIENT_H
