#include <wetman/server/lock.h>
#include <wetman/utils/filesystem.h>
#include <wetman/utils/test/macro.h>

#include <sys/wait.h>


TEST(ServerLockTest_Acquire_Success)
{
    Arena arena = Arena_New();

    char tmpDir[256];
    CREATE_TMP_DIR(tmpDir);

    Str lockPath = FS_PathJoin(Str_FromCStr(tmpDir), Str_FromCStr("server.lock"), &arena);
    bool acquired = ServerLock_Acquire(lockPath);

    EXPECT(acquired);
    EXPECT(FS_CheckExists(lockPath));

    ServerLock_Release();
    Arena_Free(&arena);
}

TEST(ServerLockTest_Acquire_DuplicateFails)
{
    Arena arena = Arena_New();

    char tmpDir[256];
    CREATE_TMP_DIR(tmpDir);

    Str lockPath = FS_PathJoin(Str_FromCStr(tmpDir), Str_FromCStr("server.lock"), &arena);
    bool firstAcquired = ServerLock_Acquire(lockPath);
    ASSERT(firstAcquired);

    pid_t pid = fork();
    ASSERT_NE(pid, -1);

    if (pid == 0) {
        bool secondAcquired = ServerLock_Acquire(lockPath);
        _exit(secondAcquired ? 0 : 1);
    }

    int status = -1;
    waitpid(pid, &status, 0);
    ASSERT(WIFEXITED(status));
    EXPECT_EQ(WEXITSTATUS(status), 1);

    ServerLock_Release();
    Arena_Free(&arena);
}

TEST(ServerLockTest_Release_CleansUpFile)
{
    Arena arena = Arena_New();

    char tmpDir[256];
    CREATE_TMP_DIR(tmpDir);

    Str lockPath = FS_PathJoin(Str_FromCStr(tmpDir), Str_FromCStr("server.lock"), &arena);
    bool acquired = ServerLock_Acquire(lockPath);
    ASSERT(acquired);
    EXPECT(FS_CheckExists(lockPath));

    ServerLock_Release();
    EXPECT(!FS_CheckExists(lockPath));

    Arena_Free(&arena);
}

TEST(ServerLockTest_AcquireAfterRelease_Success)
{
    Arena arena = Arena_New();

    char tmpDir[256];
    CREATE_TMP_DIR(tmpDir);

    Str lockPath = FS_PathJoin(Str_FromCStr(tmpDir), Str_FromCStr("server.lock"), &arena);

    bool firstAcquired = ServerLock_Acquire(lockPath);
    ASSERT(firstAcquired);
    ServerLock_Release();

    bool secondAcquired = ServerLock_Acquire(lockPath);
    EXPECT(secondAcquired);

    ServerLock_Release();
    Arena_Free(&arena);
}
