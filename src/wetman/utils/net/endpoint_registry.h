#ifndef WETMAN_UTILS_NET_ENDPOINT_REGISTRY_H
#define WETMAN_UTILS_NET_ENDPOINT_REGISTRY_H

#include <wetman/utils/data_stream.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/endpoint.h>
#include <wetman/utils/net/return_code.h>

#include <stdlib.h>


#define ENDPOINT_REGISTRY_MAX_ENDPOINT_COUNT 256


typedef struct {
    Endpoint __endpoints[ENDPOINT_REGISTRY_MAX_ENDPOINT_COUNT];
    size_t   __endpointCount;
} EndpointRegistry;

EndpointRegistry EndpointRegistry_New(void);
void EndpointRegistry_RegisterEndpoint(EndpointRegistry* endpointRegistry, Endpoint endpoint);
DataStream EndpointRegistry_CallEndpoint(
        EndpointRegistry* endpointRegistry,
        EndpointId        endpointId,
        Arena*            arena,
        DataStream*       requestData);

#endif // WETMAN_UTILS_NET_ENDPOINT_REGISTRY_H
