#ifndef WETMAN_UTILS_NET_ENDPOINT_H
#define WETMAN_UTILS_NET_ENDPOINT_H

#include <wetman/utils/data_stream.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/return_code.h>

typedef struct Message Message;

typedef struct {
    ReturnCode returnCode;
    void*      response;
} EndpointResponse;

typedef i32 EndpointId;
typedef ReturnCode(*EndpointHandler)(void* /*req*/, void* /*resp*/, Arena* /*arena*/);
typedef void(*EndpointRequestSerializer)(void*, Message*, Arena*);
typedef void(*EndpointRequestDeserializer)(void*, Message*, Arena*);
typedef void*(*EndpointRequestFactory)(Arena*);
typedef void(*EndpointResponseSerializer)(void*, Message*, Arena*);
typedef void(*EndpointResponseDeserializer)(void*, Message*, Arena*);
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

Message Endpoint_Call(
        Endpoint* endpoint,
        Arena*    arena,
        Message*  requestMessage);

#endif // WETMAN_UTILS_NET_ENDPOINT_H
