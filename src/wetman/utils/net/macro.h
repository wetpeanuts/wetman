#ifndef WETMAN_UTILS_NET_MACRO_H
#define WETMAN_UTILS_NET_MACRO_H

#include <wetman/utils/net/client.h>
#include <wetman/utils/net/endpoint.h>
#include <wetman/utils/net/message.h>

#define ENDPOINT_DECLARE_SERVER(endpointName) \
    Endpoint endpointName##_Create(void);

#define ENDPOINT_DECLARE_CLIENT(endpointName) \
    ReturnCode endpointName##_Call( \
            Client*               client, \
            endpointName##Request* request, \
            endpointName##Response* response);

#define ENDPOINT_IMPL_SERVER(endpointId, endpointName) \
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

#define ENDPOINT_IMPL_CLIENT(endpointId, endpointName) \
    ReturnCode endpointName##_Call( \
            Client*               client, \
            endpointName##Request*  request, \
            endpointName##Response* response) \
    { \
        Arena arena = Arena_New(); \
        DataStream requestBody = DataStream_New(); \
        endpointName##RequestSerializer(request, &requestBody, &arena); \
        DataStream responseData = DataStream_New(); \
        ReturnCode returnCode = Client_CallEndpoint( \
                client, \
                endpointId, \
                &arena, \
                &requestBody, \
                &responseData); \
        if (returnCode == RETURN_CODE_OK) { \
            ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseData); \
            returnCode = (ReturnCode)responseHeader.returnCode; \
            if (returnCode == RETURN_CODE_OK) { \
                endpointName##ResponseDeserializer(response, &responseData); \
            } \
        } \
        Arena_Free(&arena); \
        return returnCode; \
    }

#endif // WETMAN_UTILS_NET_MACRO_H
