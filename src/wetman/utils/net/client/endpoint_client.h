#ifndef WETMAN_UTILS_NET_CLIENT_ENDPOINT_CLIENT_H
#define WETMAN_UTILS_NET_CLIENT_ENDPOINT_CLIENT_H

#include <wetman/utils/net/client/client.h>
#include <wetman/utils/net/endpoint_registry.h>

// Creates a client instance that redirects requests
// directly to endpoint registry. Requests are handled
// in the same process. Could be useful for testing purposes.
Client EndpointClient_Connect(EndpointRegistry* endpoiuntRegistry);

#endif // WETMAN_UTILS_NET_CLIENT_ENDPOINT_CLIENT_H
