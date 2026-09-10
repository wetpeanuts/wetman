#ifndef WETMAN_UTILS_NET_MESSAGE_H
#define WETMAN_UTILS_NET_MESSAGE_H

#include <wetman/utils/data_stream.h>
#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/endpoint.h>
#include <wetman/utils/net/fd.h>
#include <wetman/utils/net/fd_stream.h>


#define REQUEST_HEADER_SERIALIZED_LEN (2 * sizeof(i32) + sizeof(RequestHeader))
#define RESPONSE_HEADER_SERIALIZED_LEN (2 * sizeof(i32) + sizeof(ResponseHeader))
#define MESSAGE_HEADER_SERIALIZED_LEN RESPONSE_HEADER_SERIALIZED_LEN

// Message couples the payload streams for a single request or response:
// header (RequestHeader or ResponseHeader), body (endpoint payload) and any
// file descriptors (fileDescriptors). Endpoint serializers and deserializers
// only ever touch the body and fileDescriptors members; the transports own
// the header stream.
typedef struct Message {
    DataStream header;
    DataStream body;
    FdStream   fileDescriptors;
} Message;

Message Message_New(void);

// Read one full message segment from the given socket: a fixed-size header
// into header, then the body (length taken from the header's msgLen field)
// into body; attached fds land in fileDescriptors.
Message Message_Read(int fd, Arena* arena);
// Write the message header + body + fds over the given socket via a single
// sendmsg (scatter/gather, no intermediate copy).
void Message_Write(Message* message, int fd);

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
