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
        .header         = DataStream_New(),
        .body           = DataStream_New(),
        .fileDescriptors = FdStream_New(),
    };
    return message;
}


static void __Message_ReadChunked(
        int         fd,
        usize       len,
        DataStream* dataStream,
        FdStream*   fds,
        Arena*      arena)
{
    dataStream->__data = Str_FromCStr("");

    char buf[__MESSAGE_BUF_LEN];
    char controlBuf[CMSG_SPACE(__MESSAGE_MAX_FDS_PER_MESSAGE * sizeof(int))];
    isize readLen = 0;

    while (readLen < (isize)len) {
        const isize chunkLen = MIN(__MESSAGE_BUF_LEN, len - (usize)readLen);
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
            dataStream->lastResult = DATA_STREAM_RESULT_FAILED_READ;
            return;
        }

        FdStream_CollectFromMessageHeader(fds, &msg, arena);

        readLen += n;
        DataSlice dataChunk = {
            .data = buf,
            .len  = (usize)n,
        };
        dataStream->__data = Str_Concat(dataStream->__data, dataChunk, arena);
    }

    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;
}

Message Message_Read(int fd, Arena* arena)
{
    Message message = Message_New();

    // Header phase: fixed-size; its msgLen tells us how long the body is.
    __Message_ReadChunked(
            fd,
            MESSAGE_HEADER_SERIALIZED_LEN,
            &message.header,
            &message.fileDescriptors,
            arena);
    if (message.header.lastResult != DATA_STREAM_RESULT_SUCCESS) {
        message.body.lastResult = DATA_STREAM_RESULT_FAILED_READ;
        return message;
    }

    DataStream headerData = DataStream_WithData(message.header.__data);
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&headerData);
    if (headerData.lastResult != DATA_STREAM_RESULT_SUCCESS) {
        message.body.lastResult = DATA_STREAM_RESULT_FAILED_READ;
        return message;
    }

    // Body phase: msgLen bytes, collecting any additional fds.
    __Message_ReadChunked(
            fd,
            (usize)responseHeader.msgLen,
            &message.body,
            &message.fileDescriptors,
            arena);

    return message;
}

void Message_Write(Message* message, int fd)
{
    char controlBuf[CMSG_SPACE(__MESSAGE_MAX_FDS_PER_MESSAGE * sizeof(int))];
    memset(controlBuf, 0, sizeof(controlBuf));

    struct iovec iov[2];
    int    iovcnt = 0;
    usize  totalLen = 0;

    if (message->header.__data.len > 0) {
        iov[iovcnt].iov_base = message->header.__data.data;
        iov[iovcnt].iov_len  = message->header.__data.len;
        totalLen += message->header.__data.len;
        iovcnt++;
    }

    if (message->body.__data.len > 0) {
        iov[iovcnt].iov_base = message->body.__data.data;
        iov[iovcnt].iov_len  = message->body.__data.len;
        totalLen += message->body.__data.len;
        iovcnt++;
    }

    // A fully empty message still needs a valid iovec for sendmsg.
    if (iovcnt == 0) {
        iov[0].iov_base = NULL;
        iov[0].iov_len  = 0;
        iovcnt = 1;
    }

    struct msghdr msg;
    memset(&msg, 0, sizeof(msg));
    msg.msg_iov = iov;
    msg.msg_iovlen = iovcnt;

    const usize fdCount = FdStream_Count(&message->fileDescriptors);
    if (fdCount > 0) {
        msg.msg_control = controlBuf;
        msg.msg_controllen = CMSG_SPACE(fdCount * sizeof(FileDescriptor));

        struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN(fdCount * sizeof(FileDescriptor));
        memcpy(CMSG_DATA(cmsg), FdStream_Data(&message->fileDescriptors), fdCount * sizeof(FileDescriptor));
    }

    isize writtenLen = sendmsg(fd, &msg, 0);
    if (writtenLen != (isize)totalLen) {
        message->body.lastResult = DATA_STREAM_RESULT_FAILED_WRITE;
        return;
    }

    message->body.lastResult = DATA_STREAM_RESULT_SUCCESS;
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

