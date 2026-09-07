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

// Message carries both the byte payload (bodyStream) and any file descriptors
// (fdStream) for a single request or response. Endpoint serializers and
// deserializers operate on this combined structure.
typedef struct Message {
    DataStream bodyStream;
    FdStream   fdStream;
} Message;

Message Message_New(void);

// Write an fd into the message's fdStream.
void Message_WriteFd(Message* message, FileDescriptor fd, Arena* arena);
// Read the next fd from the message's fdStream.
FileDescriptor Message_ReadFd(Message* message);

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
