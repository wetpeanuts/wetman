#ifndef WETMAN_UTILS_NET_CLIENT_H
#define WETMAN_UTILS_NET_CLIENT_H

#include <wetman/utils/net/endpoint.h>


typedef struct {
    const char* socketPath;
} Client;

Client Client_New(const char* socketPath);
void Client_RequestEndpoint(
        Client* client,
        EndpointId endpointId);

#endif // WETMAN_UTILS_NET_CLIENT_H
