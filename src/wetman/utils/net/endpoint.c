#include <wetman/utils/net/endpoint.h>

#include <wetman/utils/net/message.h>

#include <stdio.h>


DataStream Endpoint_Call(
        Endpoint*   endpoint,
        Arena*      arena,
        DataStream* requestData)
{
    DataStream responseData = DataStream_New();
    ResponseHeader responseHeader = {
        .returnCode = RETURN_CODE_OK,
        .msgLen     = 0,
    };

    if (endpoint->handler == NULL) {
        fprintf(stderr, "Attempt to call non initialized endpoint\n");
        responseHeader.returnCode = RETURN_CODE_ENDPOINT_NOT_INITIALIZED;
        ResponseHeader_Serialize(&responseHeader, &responseData, arena);
        return responseData;
    }

    void* request = endpoint->requestFactory(arena);
    void* response = endpoint->responseFactory(arena);
    endpoint->requestDeserializer(request, requestData);

    if (requestData->lastResult != DATA_STREAM_RESULT_SUCCESS) {
        fprintf(stderr, "Failed to parse request. Last parse error: %d\n", requestData->lastResult);
        responseHeader.returnCode = RETURN_CODE_FAILED_TO_PARSE_REQUEST;
        ResponseHeader_Serialize(&responseHeader, &responseData, arena);
        return responseData;
    }

    ReturnCode returnCode = endpoint->handler(request, response);

    DataStream responseBodyData = DataStream_New();
    endpoint->responseSerializer(response, &responseBodyData, arena);
    responseHeader.returnCode = returnCode;
    responseHeader.msgLen = responseBodyData.__data.len;

    ResponseHeader_Serialize(&responseHeader, &responseData, arena);
    DataStream_Append(&responseData, &responseBodyData, arena);

    return responseData;
}
