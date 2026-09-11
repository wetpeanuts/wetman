#ifndef WETMAN_UTILS_NET_FD_H
#define WETMAN_UTILS_NET_FD_H

#include <wetman/utils/type.h>


// FileDescriptor is a transport-level type used inside endpoint Request and
// Response structs. It represents a file descriptor that is passed over the
// wire via SCM_RIGHTS ancillary data rather than through the byte DataStream.
typedef struct {
    int fd;
} FileDescriptor;

#define FILE_DESCRIPTOR_INVALID (-1)

FileDescriptor FileDescriptor_New(int fd);

#endif // WETMAN_UTILS_NET_FD_H
