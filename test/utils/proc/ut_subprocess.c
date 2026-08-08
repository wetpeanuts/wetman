#include <wetman/utils/proc/subprocess.h>
#include <wetman/utils/test/macro.h>

#include <sys/wait.h>


void __subprocessTestCallbackSuccess(Callback* callbackMeta)
{
    (void)callbackMeta;
}

void __subprocessTestCallbackFailure(Callback* callbackMeta)
{
    (void)callbackMeta;
    exit(42);
}

TEST(SubprocessTest_RunSuccess)
{
    Callback callback = Callback_WithNoArgs(__subprocessTestCallbackSuccess);
    pid_t pid = Subprocess_Run(callback);
    int status = -1;
    waitpid(pid, &status, 0);

    ASSERT(WIFEXITED(status));
    EXPECT_EQ(WEXITSTATUS(status), 0);
}

TEST(SubprocessTest_RunFailure)
{
    Callback callback = Callback_WithNoArgs(__subprocessTestCallbackFailure);
    pid_t pid = Subprocess_Run(callback);
    int status = -1;
    waitpid(pid, &status, 0);

    ASSERT(WIFEXITED(status));
    EXPECT_EQ(WEXITSTATUS(status), 42);
}
