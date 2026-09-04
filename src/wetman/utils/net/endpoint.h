#ifndef WETMAN_UTILS_NET_ENDPOINT_H
#define WETMAN_UTILS_NET_ENDPOINT_H

#include <wetman/utils/data_stream.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/return_code.h>

typedef struct {
    ReturnCode returnCode;
    void*      response;
} EndpointResponse;

typedef i32 EndpointId;
typedef ReturnCode(*EndpointHandler)(void* /*req*/, void* /*resp*/);
typedef void(*EndpointRequestSerializer)(void*, DataStream*, Arena*);
typedef void(*EndpointRequestDeserializer)(void*, DataStream*);
typedef void*(*EndpointRequestFactory)(Arena*);
typedef void(*EndpointResponseSerializer)(void*, DataStream*, Arena*);
typedef void(*EndpointResponseDeserializer)(void*, DataStream*, Arena*);
typedef void*(*EndpointResponseFactory)(Arena*);

typedef struct {
    EndpointId                   id;
    EndpointHandler              handler;
    EndpointRequestSerializer    requestSerializer;
    EndpointRequestDeserializer  requestDeserializer;
    EndpointRequestFactory       requestFactory;
    EndpointResponseSerializer   responseSerializer;
    EndpointResponseDeserializer responseDeserializer;
    EndpointResponseFactory      responseFactory;
} Endpoint;

DataStream Endpoint_Call(
        Endpoint*   endpoint,
        Arena*      arena,
        DataStream* requestData);

#endif // WETMAN_UTILS_NET_ENDPOINT_H
