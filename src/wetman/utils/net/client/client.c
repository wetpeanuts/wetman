#include <wetman/utils/net/client/client.h>

#include <wetman/utils/net/message.h>
#include <wetman/utils/net/return_code.h>

ReturnCode Client_CallEndpoint(
        Client*     client,
        EndpointId  endpointId,
        Arena*      arena,
        DataStream* requestBody,
        DataStream* responseData)
{
    DataStream requestData = DataStream_New();
    RequestHeader requestHeader = {
        .endpointId = endpointId,
        .msgLen     = requestBody->__data.len,
    };
    RequestHeader_Serialize(&requestHeader, &requestData, arena);
    DataStream_Append(&requestData, requestBody, arena);

    *responseData = client->__requestHandler(client, &requestData, arena);

    if (responseData->lastResult != DATA_STREAM_RESULT_SUCCESS) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    DataStream responseHeaderData = *responseData;
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseHeaderData);
    if (responseHeaderData.lastResult != DATA_STREAM_RESULT_SUCCESS) {
        return RETURN_CODE_FAILED_TO_PARSE_REQUEST;
    }

    return (ReturnCode)responseHeader.returnCode;
}

