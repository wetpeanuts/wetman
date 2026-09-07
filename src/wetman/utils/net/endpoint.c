#include <wetman/utils/net/endpoint.h>

#include <wetman/utils/net/message.h>

#include <stdio.h>


Message Endpoint_Call(
        Endpoint* endpoint,
        Arena*    arena,
        Message*  requestMessage)
{
    Message responseMessage = Message_New();
    ResponseHeader responseHeader = {
        .returnCode = RETURN_CODE_OK,
        .msgLen     = 0,
    };

    if (endpoint->handler == NULL) {
        fprintf(stderr, "Attempt to call non initialized endpoint\n");
        responseHeader.returnCode = RETURN_CODE_ENDPOINT_NOT_INITIALIZED;
        ResponseHeader_Serialize(&responseHeader, &responseMessage.bodyStream, arena);
        return responseMessage;
    }

    void* request = endpoint->requestFactory(arena);
    void* response = endpoint->responseFactory(arena);
    endpoint->requestDeserializer(request, requestMessage, arena);

    if (requestMessage->bodyStream.lastResult != DATA_STREAM_RESULT_SUCCESS) {
        fprintf(stderr, "Failed to parse request. Last parse error: %d\n", requestMessage->bodyStream.lastResult);
        responseHeader.returnCode = RETURN_CODE_FAILED_TO_PARSE_REQUEST;
        ResponseHeader_Serialize(&responseHeader, &responseMessage.bodyStream, arena);
        return responseMessage;
    }

    ReturnCode returnCode = endpoint->handler(request, response);

    Message responseBodyMessage = Message_New();
    endpoint->responseSerializer(response, &responseBodyMessage, arena);
    responseHeader.returnCode = returnCode;
    responseHeader.msgLen = responseBodyMessage.bodyStream.__data.len;

    responseMessage.fdStream = responseBodyMessage.fdStream;
    ResponseHeader_Serialize(&responseHeader, &responseMessage.bodyStream, arena);
    DataStream_Append(&responseMessage.bodyStream, &responseBodyMessage.bodyStream, arena);

    return responseMessage;
}
