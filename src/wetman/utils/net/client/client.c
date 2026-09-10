#include <wetman/utils/net/client/client.h>

#include <wetman/utils/net/message.h>
#include <wetman/utils/net/return_code.h>

ReturnCode Client_CallEndpoint(
        Client*  client,
        EndpointId endpointId,
        Arena*   arena,
        Message* requestMessage,
        Message* responseMessage)
{
    RequestHeader requestHeader = {
        .endpointId = endpointId,
        .msgLen     = requestMessage->body.__data.len,
    };
    RequestHeader_Serialize(&requestHeader, &requestMessage->header, arena);

    *responseMessage = client->__requestHandler(client, requestMessage, arena);

    if (responseMessage->body.lastResult != DATA_STREAM_RESULT_SUCCESS) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseMessage->header);
    if (responseMessage->header.lastResult != DATA_STREAM_RESULT_SUCCESS) {
        return RETURN_CODE_FAILED_TO_PARSE_REQUEST;
    }

    return (ReturnCode)responseHeader.returnCode;
}

