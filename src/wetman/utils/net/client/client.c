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
    Message requestData = Message_New();
    RequestHeader requestHeader = {
        .endpointId = endpointId,
        .msgLen     = requestMessage->bodyStream.__data.len,
    };
    RequestHeader_Serialize(&requestHeader, &requestData.bodyStream, arena);
    DataStream_Append(&requestData.bodyStream, &requestMessage->bodyStream, arena);
    requestData.fdStream = requestMessage->fdStream;

    *responseMessage = client->__requestHandler(client, &requestData, arena);

    if (responseMessage->bodyStream.lastResult != DATA_STREAM_RESULT_SUCCESS) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    DataStream responseHeaderData = responseMessage->bodyStream;
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseHeaderData);
    if (responseHeaderData.lastResult != DATA_STREAM_RESULT_SUCCESS) {
        return RETURN_CODE_FAILED_TO_PARSE_REQUEST;
    }

    return (ReturnCode)responseHeader.returnCode;
}

