#ifndef WETMAN_UTILS_NET_MACRO_H
#define WETMAN_UTILS_NET_MACRO_H

#include <wetman/utils/net/endpoint.h>

#define ENDPOINT_DECLARE(endpointName) \
    ReturnCode endpointName( \
            endpointName##Request* request, \
            endpointName##Response* response); \
    \
    void endpointName##RequestSerializer( \
            endpointName##Request* request, \
            DataStream*            dataStream, \
            Arena*                 arena); \
    void endpointName##RequestDeserializer( \
            endpointName##Request* request, \
            DataStream*            dataStream); \
    \
    void endpointName##ResponseSerializer( \
            endpointName##Response* response, \
            DataStream*             dataStream, \
            Arena*                  arena); \
    void endpointName##ResponseDeserializer( \
            endpointName##Response* response, \
            DataStream*             dataStream); \
    \
    Endpoint endpointName##_Create(void);

#define ENDPOINT_IMPL(endpointId, endpointName) \
    ReturnCode __Endpoint_##endpointName(void* request, void* response) { \
        return endpointName( \
                (endpointName##Request*)request, \
                (endpointName##Response*)response); \
    } \
    \
    void __Endpoint_##endpointName##RequestSerializer(void* req, DataStream* ds, Arena* arena) \
    { \
        endpointName##RequestSerializer((endpointName##Request*)req, ds, arena); \
    } \
    \
    void __Endpoint_##endpointName##RequestDeserializer(void* req, DataStream* ds) \
    { \
        endpointName##RequestDeserializer((endpointName##Request*)req, ds); \
    } \
    \
    void* __Endpoint_##endpointName##RequestFactory(Arena* arena) \
    { \
        return Arena_Alloc(arena, sizeof(endpointName##Request)); \
    } \
    \
    void __Endpoint_##endpointName##ResponseSerializer(void* resp, DataStream* ds, Arena* arena) \
    { \
        endpointName##ResponseSerializer((endpointName##Response*)resp, ds, arena); \
    } \
    \
    void __Endpoint_##endpointName##ResponseDeserializer(void* resp, DataStream* ds) \
    { \
        endpointName##ResponseDeserializer((endpointName##Response*)resp, ds); \
    } \
    \
    void* __Endpoint_##endpointName##ResponseFactory(Arena* arena) \
    { \
        return Arena_Alloc(arena, sizeof(endpointName##Response)); \
    } \
    \
    Endpoint endpointName##_Create(void) \
    { \
        Endpoint endpoint = { \
            .id                   = endpointId, \
            .handler              = __Endpoint_##endpointName, \
            .requestSerializer    = __Endpoint_##endpointName##RequestSerializer, \
            .requestDeserializer  = __Endpoint_##endpointName##RequestDeserializer, \
            .requestFactory       = __Endpoint_##endpointName##RequestFactory, \
            .responseSerializer   = __Endpoint_##endpointName##ResponseSerializer, \
            .responseDeserializer = __Endpoint_##endpointName##ResponseDeserializer, \
            .responseFactory      = __Endpoint_##endpointName##ResponseFactory, \
        }; \
        return endpoint; \
    }
#endif // WETMAN_UTILS_NET_MACRO_H
