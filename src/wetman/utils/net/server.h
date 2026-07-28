#ifndef WETMAN_UTILS_NET_SERVER_H
#define WETMAN_UTILS_NET_SERVER_H

#include <wetman/utils/net/endpoint_registry.h>


int Server_Run(const char* socketPath, EndpointRegistry* endpointRegistry);

#endif // WETMAN_UTILS_NET_SERVER_H
