#ifndef WETMAN_UTILS_NET_SERVER_NODE_H
#define WETMAN_UTILS_NET_SERVER_NODE_H

#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/endpoint.h>
#include <wetman/utils/net/return_code.h>

#include <stdlib.h>


#define SERVER_NODE_MAX_ENDPOINT_COUNT 256


typedef struct {
    Endpoint endpoints[SERVER_NODE_MAX_ENDPOINT_COUNT];
    size_t endpointCount;
} ServerNode;

ServerNode ServerNode_New(void);
void ServerNode_RegisterEndpoint(ServerNode* serverNode, Endpoint endpoint);
ReturnCode ServerNode_CallEndpoint(
        ServerNode* serverNode,
        EndpointId endpointId,
        Arena* arena,
        Str data);

#endif // WETMAN_UTILS_NET_SERVER_NODE_H
