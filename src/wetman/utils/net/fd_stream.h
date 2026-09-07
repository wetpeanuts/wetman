#ifndef WETMAN_UTILS_NET_FD_STREAM_H
#define WETMAN_UTILS_NET_FD_STREAM_H

#include <wetman/utils/mem/arena.h>
#include <wetman/utils/type.h>


// FdStream holds a growable, arena-backed array of file descriptors alongside
// the byte DataStream. FDs never travel through the byte stream; they are
// passed via SCM_RIGHTS ancillary data, and this stream mirrors their order.
typedef struct {
    i32*  data;
    usize len;      // number of fds currently in the stream
    usize readPos;  // cursor advanced by FdStream_Pop
} FdStream;

FdStream FdStream_New(void);

int FdStream_Push(FdStream* fdStream, int fd, Arena* arena);
int FdStream_Pop(FdStream* fdStream);
usize FdStream_Count(FdStream* fdStream);
i32* FdStream_Data(FdStream* fdStream);

#endif // WETMAN_UTILS_NET_FD_STREAM_H
