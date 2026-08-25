#include <wetman/server/lock.h>
#include <wetman/utils/filesystem.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

static int  __serverLockFd     = -1;
static Str  __serverLockPath   = {0};

bool ServerLock_Acquire(Str lockFilePath)
{
    __serverLockFd = FS_OpenFile(lockFilePath, O_CREAT | O_RDWR | O_EXLOCK | O_NONBLOCK);
    if (__serverLockFd < 0) {
        return false;
    }
    __serverLockPath = lockFilePath;
    return true;
}

void ServerLock_Release(void)
{
    if (__serverLockFd >= 0) {
        close(__serverLockFd);
        __serverLockFd = -1;
    }
    if (__serverLockPath.data) {
        char buf[4096];
        snprintf(buf, sizeof(buf), "%.*s", (i32)__serverLockPath.len, __serverLockPath.data);
        unlink(buf);
        __serverLockPath = (Str){0};
    }
}
