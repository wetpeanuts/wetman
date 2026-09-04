#include <wetman/utils/net/endpoint_registry.h>

#include <wetman/utils/net/message.h>

#include <assert.h>
#include <stdio.h>


EndpointRegistry EndpointRegistry_New(void)
{
    EndpointRegistry node = {
        .__endpointCount = 0,
    };

    Endpoint emptyEndpoint = {
        .id                   = -1,
        .handler              = NULL,
        .requestSerializer    = NULL,
        .requestDeserializer  = NULL,
        .responseSerializer   = NULL,
        .responseDeserializer = NULL,
    };

    for (size_t i = 0; i < ENDPOINT_REGISTRY_MAX_ENDPOINT_COUNT; ++i) {
        node.__endpoints[i] = emptyEndpoint;
    }

    return node;
}

void EndpointRegistry_RegisterEndpoint(EndpointRegistry* endpointRegistry, Endpoint endpoint)
{
    assert(0 <= endpoint.id && endpoint.id < ENDPOINT_REGISTRY_MAX_ENDPOINT_COUNT);
    assert(endpoint.handler != NULL);
    assert(endpoint.requestSerializer != NULL);
    assert(endpoint.requestDeserializer != NULL);
    assert(endpoint.responseSerializer != NULL);
    assert(endpoint.responseDeserializer != NULL);

    if (endpointRegistry->__endpoints[endpoint.id].id != -1) {
        fprintf(stderr, "Endpoint with ID %d is already configured\n", endpoint.id);
        return;
    }

    endpointRegistry->__endpoints[endpoint.id] = endpoint;
    endpointRegistry->__endpointCount++;
}

DataStream EndpointRegistry_CallEndpoint(
        EndpointRegistry* endpointRegistry,
        EndpointId        endpointId,
        Arena*            arena,
        DataStream*       requestData)
{
    DataStream responseData = DataStream_New();
    ResponseHeader responseHeader = {
        .returnCode = RETURN_CODE_OK,
        .msgLen     = 0,
    };

    if (0 > endpointId || endpointId >= ENDPOINT_REGISTRY_MAX_ENDPOINT_COUNT) {
        fprintf(stderr, "Attempt to call endpoint with invalid ID: %d. Available ID range: [0-%d]\n",
                endpointId, ENDPOINT_REGISTRY_MAX_ENDPOINT_COUNT - 1);
        responseHeader.returnCode = RETURN_CODE_INVALID_ENDPOINT_ID;
        ResponseHeader_Serialize(&responseHeader, &responseData, arena);
        return responseData;
    }

    if (endpointRegistry->__endpoints[endpointId].id != endpointId) {
        fprintf(stderr, "Attempt to call non existent endpoint: ID = %d\n", endpointId);
        responseHeader.returnCode = RETURN_CODE_ENDPOINT_NOT_INITIALIZED;
        ResponseHeader_Serialize(&responseHeader, &responseData, arena);
        return responseData;
    }

    Endpoint endpoint = endpointRegistry->__endpoints[endpointId];
    void* request = endpoint.requestFactory(arena);
    void* response = endpoint.responseFactory(arena);
    endpoint.requestDeserializer(request, requestData, arena);

    if (requestData->lastResult != DATA_STREAM_RESULT_SUCCESS) {
        fprintf(stderr, "Failed to parse request. Last parse error: %d\n", requestData->lastResult);
        responseHeader.returnCode = RETURN_CODE_FAILED_TO_PARSE_REQUEST;
        ResponseHeader_Serialize(&responseHeader, &responseData, arena);
        return responseData;
    }

    ReturnCode returnCode = endpoint.handler(request, response);

    DataStream responseBodyData = DataStream_New();
    endpoint.responseSerializer(response, &responseBodyData, arena);
    responseHeader.returnCode = returnCode;
    responseHeader.msgLen = responseBodyData.__data.len;

    ResponseHeader_Serialize(&responseHeader, &responseData, arena);
    DataStream_Append(&responseData, &responseBodyData, arena);

    return responseData;
}
