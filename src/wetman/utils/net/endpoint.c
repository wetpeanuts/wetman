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
        ResponseHeader_Serialize(&responseHeader, &responseMessage.header, arena);
        return responseMessage;
    }

    void* request = endpoint->requestFactory(arena);
    void* response = endpoint->responseFactory(arena);
    endpoint->requestDeserializer(request, requestMessage, arena);

    if (requestMessage->body.lastResult != DATA_STREAM_RESULT_SUCCESS) {
        fprintf(stderr, "Failed to parse request. Last parse error: %d\n", requestMessage->body.lastResult);
        responseHeader.returnCode = RETURN_CODE_FAILED_TO_PARSE_REQUEST;
        ResponseHeader_Serialize(&responseHeader, &responseMessage.header, arena);
        return responseMessage;
    }

    ReturnCode returnCode = endpoint->handler(request, response, arena);

    endpoint->responseSerializer(response, &responseMessage, arena);
    responseHeader.returnCode = returnCode;
    responseHeader.msgLen = responseMessage.body.__data.len;
    ResponseHeader_Serialize(&responseHeader, &responseMessage.header, arena);

    return responseMessage;
}
