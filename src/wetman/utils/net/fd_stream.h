#ifndef WETMAN_UTILS_NET_FD_STREAM_H
#define WETMAN_UTILS_NET_FD_STREAM_H

#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/fd.h>
#include <wetman/utils/type.h>


// FdStream holds a growable, arena-backed array of file descriptors alongside
// the byte DataStream. FDs never travel through the byte stream; they are
// passed via SCM_RIGHTS ancillary data, and this stream mirrors their order.
typedef struct {
    FileDescriptor* data;
    usize           len;      // number of fds currently in the stream
    usize           readPos;  // cursor advanced by FdStream_Pop
} FdStream;

FdStream FdStream_New(void);

int FdStream_Push(FdStream* fdStream, FileDescriptor fd, Arena* arena);
FileDescriptor FdStream_Pop(FdStream* fdStream);
usize FdStream_Count(FdStream* fdStream);
FileDescriptor* FdStream_Data(FdStream* fdStream);

struct msghdr;
// Collect the SCM_RIGHTS file descriptors attached to a received msghdr,
// pushing them onto the end of fdStream.
void FdStream_CollectFromMessageHeader(FdStream* fdStream, struct msghdr* msg, Arena* arena);

#endif // WETMAN_UTILS_NET_FD_STREAM_H
