#include <wetman/utils/proc/subprocess.h>
#include <wetman/utils/test/macro.h>


TEST(IntegrationTest_HealthCheck_NoServerRunning)
{
    pid_t clientPid = Subprocess_RunCommand("../build/wetman healthcheck");
    ASSERT_NE(clientPid, -1);

    i32 statusCode = Subprocess_WaitFor(clientPid, 5000);
    EXPECT_EQ(statusCode, 1);
}

TEST(IntegrationTest_HealthCheck_Success)
{
    pid_t clientPid = Subprocess_RunCommand("../build/wetman healthcheck");
    ASSERT_NE(clientPid, -1);

    i32 statusCode = Subprocess_WaitFor(clientPid, 5000);
    EXPECT_EQ(statusCode, 0);
}

