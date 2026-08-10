#ifndef WETMAN_UTILS_NET_MESSAGE_H
#define WETMAN_UTILS_NET_MESSAGE_H

#include <wetman/utils/data_stream.h>
#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/endpoint.h>


#define REQUEST_HEADER_SERIALIZED_LEN (2 * sizeof(i32) + sizeof(RequestHeader))
#define RESPONSE_HEADER_SERIALIZED_LEN (2 * sizeof(i32) + sizeof(ResponseHeader))

typedef struct {
    EndpointId endpointId;
    u32        msgLen;
} RequestHeader;

void RequestHeader_Serialize(
        RequestHeader const* requestHeader,
        DataStream*          dataStream,
        Arena*               arena);
RequestHeader RequestHeader_Deserialize(DataStream* dataStream);

typedef struct {
    i32 returnCode;
    u32 msgLen;
} ResponseHeader;

void ResponseHeader_Serialize(
        ResponseHeader const* responseHeader,
        DataStream*           dataStream,
        Arena*                arena);
ResponseHeader ResponseHeader_Deserialize(DataStream* dataStream);

#endif // WETMAN_UTILS_NET_MESSAGE_H
