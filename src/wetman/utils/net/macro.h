#ifndef WETMAN_UTILS_NET_MACRO_H
#define WETMAN_UTILS_NET_MACRO_H

#include <wetman/utils/net/endpoint.h>

#define DELCARE_ENDPOINT(endpointId, endpointName) \
    ReturnCode __Endpoint_##endpointName(void* request, void* response) { \
        return endpointName( \
                (endpointName##Request*)request, \
                (endpointName##Response*)response); \
    } \
    \
    Endpoint TestEndpoint_Init(void) \
    { \
        Endpoint endpoint = { \
            .id              = endpointId, \
            .handler         = __Endpoint_##endpointName, \
            .requestParser   = endpointName##RequestParser, \
            .responseFactory = endpointName##ResponseFactory, \
        }; \
        return endpoint; \
    }
#endif // WETMAN_UTILS_NET_MACRO_H
