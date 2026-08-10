#ifndef WETMAN_UTILS_NET_ENDPOINT_H
#define WETMAN_UTILS_NET_ENDPOINT_H

#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/return_code.h>


typedef i32 EndpointId;
typedef ReturnCode(*EndpointHandler)(void*,void*);
typedef void*(*EndpointRequestParser)(Arena*,Str);
typedef void*(*EndpointResponseFactory)(Arena*);
// typedef Str(*EndpointResponseSerializer)(void*, Arena*);

typedef struct {
    EndpointId              id;
    EndpointHandler         handler;
    EndpointRequestParser   requestParser;
    EndpointResponseFactory responseFactory;
} Endpoint;

#endif // WETMAN_UTILS_NET_ENDPOINT_H
