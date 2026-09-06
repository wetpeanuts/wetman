#include <wetman/utils/test/macro.h>

#include "it_healthcheck.c"
#include "it_workspace.c"

// Tmp dir macros work only in the context of the test case,
// so we need to wrap it into a separate test
static pid_t __serverPid;

TEST(IntegrationTest_RunServer)
{
    char wdir[256];
    CREATE_TMP_DIR(wdir);

    char serverCmd[512];
    snprintf(serverCmd, sizeof(serverCmd),
            "WETMAN_WDIR=%s ../build/wetman_server", wdir);

    __serverPid = Subprocess_RunCommand(serverCmd);
    ASSERT_NE(__serverPid, -1);

    sleep(1);

    // Check that second server instance cannot be run
    pid_t secondServerPid = Subprocess_RunCommand(serverCmd);
    ASSERT_NE(__serverPid, -1);

    i32 statusCode = Subprocess_WaitFor(secondServerPid, 5000);
    ASSERT_EQ(statusCode, 1);
}

TEST(IntegrationTest_ShutDownServer)
{
    ASSERT(Subprocess_Kill(__serverPid));
}

void registerIntegrationTests(void)
{
    // Tests without running server
    REGISTER_TEST(IntegrationTest_HealthCheck_NoServerRunning);
    REGISTER_TEST(IntegrationTest_Workspace_NoServerRunning);

    // Tests required server instance running
    REGISTER_TEST(IntegrationTest_RunServer);

    REGISTER_TEST(IntegrationTest_HealthCheck_Success);
    REGISTER_TEST(IntegrationTest_Workspace_InitDelete_DefaultArgs);
    REGISTER_TEST(IntegrationTest_Workspace_InitDelete_NamedArgs);

    REGISTER_TEST(IntegrationTest_ShutDownServer);
}
