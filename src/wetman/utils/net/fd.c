#include <wetman/utils/net/fd.h>


FileDescriptor FileDescriptor_New(int fd)
{
    FileDescriptor fileDescriptor = {
        .fd = fd,
    };
    return fileDescriptor;
}
