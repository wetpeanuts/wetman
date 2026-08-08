#include <wetman/utils/net/message.h>


void RequestHeader_Serialize(
        RequestHeader const* requestHeader,
        DataStream*          dataStream,
        Arena*               arena)
{
    DataStream_PushI32(dataStream, requestHeader->endpointId, arena);
}

RequestHeader RequestHeader_Deserialize(DataStream* dataStream)
{
    const i32 endpointId = DataStream_PopI32(dataStream);

    RequestHeader messageHeader = {
        .endpointId = endpointId,
    };

    return messageHeader;
}

void ResponseHeader_Serialize(
        ResponseHeader const* responseHeader,
        DataStream*           dataStream,
        Arena*                arena)
{
    DataStream_PushI32(dataStream, responseHeader->returnCode, arena);
}

ResponseHeader ResponseHeader_Deserialize(DataStream* dataStream)
{
    const i32 returnCode = DataStream_PopI32(dataStream);

    ResponseHeader responseHeader = {
        .returnCode = returnCode,
    };

    return responseHeader;
}

