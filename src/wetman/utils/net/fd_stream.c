#include <wetman/utils/net/fd_stream.h>

#include <wetman/utils/macro.h>

#include <stdlib.h>


FdStream FdStream_New(void)
{
    FdStream fdStream = {
        .data    = NULL,
        .len     = 0,
        .readPos = 0,
    };
    return fdStream;
}

int FdStream_Push(FdStream* fdStream, int fd, Arena* arena)
{
    i32* newData = (i32*)Arena_Alloc(arena, (fdStream->len + 1) * sizeof(i32));
    if (!newData) {
        return -1;
    }

    for (usize i = 0; i < fdStream->len; i++) {
        newData[i] = fdStream->data[i];
    }
    newData[fdStream->len] = (i32)fd;

    fdStream->data = newData;
    fdStream->len++;
    return 0;
}

int FdStream_Pop(FdStream* fdStream)
{
    if (UNLIKELY(fdStream->readPos >= fdStream->len)) {
        return -1;
    }

    int fd = (int)fdStream->data[fdStream->readPos];
    fdStream->readPos++;
    return fd;
}

usize FdStream_Count(FdStream* fdStream)
{
    return fdStream->len - fdStream->readPos;
}

i32* FdStream_Data(FdStream* fdStream)
{
    return fdStream->data + fdStream->readPos;
}
