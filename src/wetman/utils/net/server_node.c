#include <wetman/utils/net/server_node.h>

#include <assert.h>
#include <stdio.h>


ServerNode ServerNode_New()
{
    ServerNode node = {
        .endpointCount = 0,
    };

    Endpoint emptyEndpoint = {
        .id = -1,
        .handler = NULL,
        .requestParser = NULL,
        .responseFactory = NULL,
    };

    for (size_t i = 0; i < SERVER_NODE_MAX_ENDPOINT_COUNT; ++i) {
        node.endpoints[i] = emptyEndpoint;
    }

    return node;
}

void ServerNode_RegisterEndpoint(ServerNode* serverNode, Endpoint endpoint)
{
    assert(0 <= endpoint.id && endpoint.id < SERVER_NODE_MAX_ENDPOINT_COUNT);
    assert(endpoint.handler != NULL);
    assert(endpoint.requestParser != NULL);
    assert(endpoint.responseFactory != NULL);

    if (serverNode->endpoints[endpoint.id].id != -1) {
        fprintf(stderr, "Endpoint with ID %d is already configured", endpoint.id);
        return;
    }

    serverNode->endpoints[endpoint.id] = endpoint;
    serverNode->endpointCount++;
}

ReturnCode ServerNode_CallEndpoint(
        ServerNode* serverNode,
        EndpointId endpointId,
        Arena* arena,
        Str data)
{
    if (0 < endpointId || endpointId >= SERVER_NODE_MAX_ENDPOINT_COUNT) {
        fprintf(stderr, "Attempt to call endpoint with invalid ID: %d. Available ID range: [0-%d]",
                endpointId, SERVER_NODE_MAX_ENDPOINT_COUNT - 1);
        return RETURN_CODE_INVALID_ENDPOINT_ID;
    }

    if (serverNode->endpoints[endpointId].id != endpointId) {
        fprintf(stderr, "Attempt to call non existent endpoint: ID = %d", endpointId);
        return RETURN_CODE_ENDPOINT_NOT_INITIALIZED;
    }

    Endpoint endpoint = serverNode->endpoints[endpointId];
    void* request = endpoint.requestParser(arena, data);
    void* response = endpoint.responseFactory(arena);

    return endpoint.handler(request, response);
}
