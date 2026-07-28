#include <wetman/utils/net/endpoint_registry.h>

#include <assert.h>
#include <stdio.h>


EndpointRegistry EndpointRegistry_New(void)
{
    EndpointRegistry node = {
        .__endpointCount = 0,
    };

    Endpoint emptyEndpoint = {
        .id = -1,
        .handler = NULL,
        .requestParser = NULL,
        .responseFactory = NULL,
    };

    for (size_t i = 0; i < ENDPOINT_REGISTRY_MAX_ENDPOINT_COUNT; ++i) {
        node.__endpoints[i] = emptyEndpoint;
    }

    return node;
}

void EndpointRegistry_RegisterEndpoint(EndpointRegistry* serverNode, Endpoint endpoint)
{
    assert(0 <= endpoint.id && endpoint.id < ENDPOINT_REGISTRY_MAX_ENDPOINT_COUNT);
    assert(endpoint.handler != NULL);
    assert(endpoint.requestParser != NULL);
    assert(endpoint.responseFactory != NULL);

    if (serverNode->__endpoints[endpoint.id].id != -1) {
        fprintf(stderr, "Endpoint with ID %d is already configured", endpoint.id);
        return;
    }

    serverNode->__endpoints[endpoint.id] = endpoint;
    serverNode->__endpointCount++;
}

ReturnCode EndpointRegistry_CallEndpoint(
        EndpointRegistry* serverNode,
        EndpointId        endpointId,
        Arena*            arena,
        Str               data)
{
    if (0 < endpointId || endpointId >= ENDPOINT_REGISTRY_MAX_ENDPOINT_COUNT) {
        fprintf(stderr, "Attempt to call endpoint with invalid ID: %d. Available ID range: [0-%d]",
                endpointId, ENDPOINT_REGISTRY_MAX_ENDPOINT_COUNT - 1);
        return RETURN_CODE_INVALID_ENDPOINT_ID;
    }

    if (serverNode->__endpoints[endpointId].id != endpointId) {
        fprintf(stderr, "Attempt to call non existent endpoint: ID = %d", endpointId);
        return RETURN_CODE_ENDPOINT_NOT_INITIALIZED;
    }

    Endpoint endpoint = serverNode->__endpoints[endpointId];
    void* request = endpoint.requestParser(arena, data);
    void* response = endpoint.responseFactory(arena);

    return endpoint.handler(request, response);
}
