#include <wetman/utils/net/message.h>

#include <wetman/utils/macro.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>


#define __MESSAGE_BUF_LEN 256
#define __MESSAGE_MAX_FDS_PER_MESSAGE 64


Message Message_New(void)
{
    Message message = {
        .bodyStream = DataStream_New(),
        .fdStream   = FdStream_New(),
    };
    return message;
}

Message Message_Read(int fd, Arena* arena, isize maxLen)
{
    Message message = Message_New();

    DataSlice data = Str_FromCStr("");
    char buf[__MESSAGE_BUF_LEN];
    char controlBuf[CMSG_SPACE(__MESSAGE_MAX_FDS_PER_MESSAGE * sizeof(int))];
    isize readLen = 0;

    while (readLen < maxLen) {
        const isize chunkLen = MIN(__MESSAGE_BUF_LEN, maxLen - readLen);
        struct iovec iov = {
            .iov_base = buf,
            .iov_len  = (usize)chunkLen,
        };

        struct msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = controlBuf;
        msg.msg_controllen = sizeof(controlBuf);

        isize n = recvmsg(fd, &msg, 0);
        if (n <= 0) {
            message.bodyStream.lastResult = DATA_STREAM_RESULT_FAILED_READ;
            break;
        }

        FdStream_CollectFromMessageHeader(&message.fdStream, &msg, arena);

        readLen += n;
        DataSlice dataChunk = {
            .data = buf,
            .len  = (usize)n,
        };
        data = Str_Concat(data, dataChunk, arena);
        message.bodyStream.lastResult = DATA_STREAM_RESULT_SUCCESS;
    }

    message.bodyStream.__data = data;
    return message;
}

void Message_Write(Message* message, int fd)
{
    char controlBuf[CMSG_SPACE(__MESSAGE_MAX_FDS_PER_MESSAGE * sizeof(int))];
    memset(controlBuf, 0, sizeof(controlBuf));

    struct iovec iov = {
        .iov_base = message->bodyStream.__data.data,
        .iov_len  = message->bodyStream.__data.len,
    };

    struct msghdr msg;
    memset(&msg, 0, sizeof(msg));
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    const usize fdCount = FdStream_Count(&message->fdStream);
    if (fdCount > 0) {
        msg.msg_control = controlBuf;
        msg.msg_controllen = CMSG_SPACE(fdCount * sizeof(FileDescriptor));

        struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN(fdCount * sizeof(FileDescriptor));
        memcpy(CMSG_DATA(cmsg), FdStream_Data(&message->fdStream), fdCount * sizeof(FileDescriptor));
    }

    isize writtenLen = sendmsg(fd, &msg, 0);
    if (writtenLen != (isize)message->bodyStream.__data.len) {
        message->bodyStream.lastResult = DATA_STREAM_RESULT_FAILED_WRITE;
        return;
    }

    message->bodyStream.lastResult = DATA_STREAM_RESULT_SUCCESS;
}

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

