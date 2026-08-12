#include <wetman/utils/net/client.h>

#include <wetman/utils/net/message.h>
#include <wetman/utils/net/return_code.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>


Client Client_New(const char* socketPath)
{
    Client client = {
        .request                = ClientTransport_UnixRequest,
        .unixContext.socketPath = socketPath,
    };

    return client;
}

DataStream ClientTransport_UnixRequest(
        Client*     client,
        DataStream* requestData,
        Arena*      arena)
{
    DataStream responseData = DataStream_New();

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, client->unixContext.socketPath, sizeof(addr.sun_path) - 1);

    const int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("Failed to create socket");
        responseData.lastResult = DATA_STREAM_RESULT_FAILED_READ;
        return responseData;
    }

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Failed to connect to server");
        close(fd);
        responseData.lastResult = DATA_STREAM_RESULT_FAILED_READ;
        return responseData;
    }

    DataStream_Write(requestData, fd);
    if (requestData->lastResult != DATA_STREAM_RESULT_SUCCESS) {
        perror("Failed to write request");
        close(fd);
        responseData.lastResult = DATA_STREAM_RESULT_FAILED_WRITE;
        return responseData;
    }

    DataStream headerData = DataStream_Read(fd, arena, RESPONSE_HEADER_SERIALIZED_LEN);

    DataStream headerCopy = headerData;
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&headerCopy);

    DataStream bodyData = DataStream_Read(fd, arena, responseHeader.msgLen);

    close(fd);

    DataStream_Append(&responseData, &headerData, arena);
    DataStream_Append(&responseData, &bodyData, arena);

    return responseData;
}

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

    *responseData = client->request(client, &requestData, arena);

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
