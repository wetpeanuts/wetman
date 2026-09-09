#include <wetman/utils/net/fd_stream.h>

#include <wetman/utils/macro.h>

#include <sys/socket.h>


FdStream FdStream_New(void)
{
    FdStream fdStream = {
        .data    = NULL,
        .len     = 0,
        .readPos = 0,
    };
    return fdStream;
}

int FdStream_Push(FdStream* fdStream, FileDescriptor fd, Arena* arena)
{
    FileDescriptor* newData = (FileDescriptor*)Arena_Alloc(
            arena, (fdStream->len + 1) * sizeof(FileDescriptor));
    if (!newData) {
        return -1;
    }

    for (usize i = 0; i < fdStream->len; i++) {
        newData[i] = fdStream->data[i];
    }
    newData[fdStream->len] = fd;

    fdStream->data = newData;
    fdStream->len++;
    return 0;
}

FileDescriptor FdStream_Pop(FdStream* fdStream)
{
    if (UNLIKELY(fdStream->readPos >= fdStream->len)) {
        return FileDescriptor_New(FILE_DESCRIPTOR_INVALID);
    }

    FileDescriptor fd = fdStream->data[fdStream->readPos];
    fdStream->readPos++;
    return fd;
}

usize FdStream_Count(FdStream* fdStream)
{
    return fdStream->len - fdStream->readPos;
}

FileDescriptor* FdStream_Data(FdStream* fdStream)
{
    return fdStream->data + fdStream->readPos;
}

void FdStream_CollectFromMessageHeader(FdStream* fdStream, struct msghdr* msg, Arena* arena)
{
    struct cmsghdr* cmsg;
    for (cmsg = CMSG_FIRSTHDR(msg); cmsg != NULL;
            cmsg = CMSG_NXTHDR(msg, cmsg)) {
        if (cmsg->cmsg_level != SOL_SOCKET || cmsg->cmsg_type != SCM_RIGHTS) {
            continue;
        }

        const usize fdCount = (cmsg->cmsg_len - CMSG_LEN(0)) / sizeof(int);
        int* fds = (int*)CMSG_DATA(cmsg);
        for (usize i = 0; i < fdCount; i++) {
            FdStream_Push(fdStream, FileDescriptor_New(fds[i]), arena);
        }
    }
}
