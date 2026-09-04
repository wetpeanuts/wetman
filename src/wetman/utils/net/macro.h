#ifndef WETMAN_UTILS_NET_MACRO_H
#define WETMAN_UTILS_NET_MACRO_H

#include <wetman/utils/net/client/client.h>
#include <wetman/utils/net/endpoint.h>
#include <wetman/utils/net/message.h>

#define ENDPOINT_DECLARE_SERVER(endpointName) \
    Endpoint endpointName##_Create(void);

#define ENDPOINT_DECLARE_CLIENT(endpointName) \
    ReturnCode endpointName##_Call( \
            Client*               client, \
            endpointName##_Request* request, \
            endpointName##_Response* response, \
            Arena*                arena);

#define ENDPOINT_IMPL_SERVER(endpointId, endpointName) \
    ReturnCode __Endpoint_##endpointName(void* request, void* response) { \
        return endpointName( \
                (endpointName##_Request*)request, \
                (endpointName##_Response*)response); \
    } \
    \
    void __Endpoint_##endpointName##_RequestSerializer(void* req, DataStream* ds, Arena* arena) \
    { \
        endpointName##_RequestSerializer((endpointName##_Request*)req, ds, arena); \
    } \
    \
    void __Endpoint_##endpointName##_RequestDeserializer(void* req, DataStream* ds, Arena* arena) \
    { \
        endpointName##_RequestDeserializer((endpointName##_Request*)req, ds, arena); \
    } \
    \
    void* __Endpoint_##endpointName##_RequestFactory(Arena* arena) \
    { \
        return Arena_Alloc(arena, sizeof(endpointName##_Request)); \
    } \
    \
    void __Endpoint_##endpointName##_ResponseSerializer(void* resp, DataStream* ds, Arena* arena) \
    { \
        endpointName##_ResponseSerializer((endpointName##_Response*)resp, ds, arena); \
    } \
    \
    void __Endpoint_##endpointName##_ResponseDeserializer(void* resp, DataStream* ds, Arena* arena) \
    { \
        endpointName##_ResponseDeserializer((endpointName##_Response*)resp, ds, arena); \
    } \
    \
    void* __Endpoint_##endpointName##_ResponseFactory(Arena* arena) \
    { \
        return Arena_Alloc(arena, sizeof(endpointName##_Response)); \
    } \
    \
    Endpoint endpointName##_Create(void) \
    { \
        Endpoint endpoint = { \
            .id                   = endpointId, \
            .handler              = __Endpoint_##endpointName, \
            .requestSerializer    = __Endpoint_##endpointName##_RequestSerializer, \
            .requestDeserializer  = __Endpoint_##endpointName##_RequestDeserializer, \
            .requestFactory       = __Endpoint_##endpointName##_RequestFactory, \
            .responseSerializer   = __Endpoint_##endpointName##_ResponseSerializer, \
            .responseDeserializer = __Endpoint_##endpointName##_ResponseDeserializer, \
            .responseFactory      = __Endpoint_##endpointName##_ResponseFactory, \
        }; \
        return endpoint; \
    }

#define ENDPOINT_IMPL_CLIENT(endpointId, endpointName) \
    ReturnCode endpointName##_Call( \
            Client*                  client, \
            endpointName##_Request*  request, \
            endpointName##_Response* response, \
            Arena*                   arena) \
    { \
        DataStream requestBody = DataStream_New(); \
        endpointName##_RequestSerializer(request, &requestBody, arena); \
        DataStream responseData = DataStream_New(); \
        ReturnCode returnCode = Client_CallEndpoint( \
                client, \
                endpointId, \
                arena, \
                &requestBody, \
                &responseData); \
        if (returnCode == RETURN_CODE_OK) { \
            ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseData); \
            returnCode = (ReturnCode)responseHeader.returnCode; \
            if (returnCode == RETURN_CODE_OK) { \
                endpointName##_ResponseDeserializer(response, &responseData, arena); \
            } \
        } \
        return returnCode; \
    }

#endif // WETMAN_UTILS_NET_MACRO_H
