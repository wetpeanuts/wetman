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
    char wdir[256];
    CREATE_TMP_DIR(wdir);

    char serverCmd[512];
    snprintf(serverCmd, sizeof(serverCmd),
            "WETMAN_WDIR=%s ../build/wetman_server", wdir);

    pid_t serverPid = Subprocess_RunCommand(serverCmd);
    ASSERT_NE(serverPid, -1);

    sleep(1);

    pid_t clientPid = Subprocess_RunCommand("../build/wetman healthcheck");
    ASSERT_NE(clientPid, -1);

    i32 statusCode = Subprocess_WaitFor(clientPid, 5000);
    EXPECT_EQ(statusCode, 0);

    ASSERT(Subprocess_Kill(serverPid));
}

