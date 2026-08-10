#include <wetman/utils/net/message.h>


void RequestHeader_Serialize(
        RequestHeader const* requestHeader,
        DataStream*          dataStream,
        Arena*               arena)
{
    DataStream_PushI32(dataStream, requestHeader->endpointId, arena);
    DataStream_PushU32(dataStream, requestHeader->msgLen, arena);
}

RequestHeader RequestHeader_Deserialize(DataStream* dataStream)
{
    const i32 endpointId = DataStream_PopI32(dataStream);
    const u32 msgLen     = DataStream_PopU32(dataStream);

    RequestHeader messageHeader = {
        .endpointId = endpointId,
        .msgLen     = msgLen,
    };

    return messageHeader;
}

void ResponseHeader_Serialize(
        ResponseHeader const* responseHeader,
        DataStream*           dataStream,
        Arena*                arena)
{
    DataStream_PushI32(dataStream, responseHeader->returnCode, arena);
    DataStream_PushU32(dataStream, responseHeader->msgLen, arena);
}

ResponseHeader ResponseHeader_Deserialize(DataStream* dataStream)
{
    const i32 returnCode = DataStream_PopI32(dataStream);
    const u32 msgLen     = DataStream_PopU32(dataStream);

    ResponseHeader responseHeader = {
        .returnCode = returnCode,
        .msgLen     = msgLen,
    };

    return responseHeader;
}

