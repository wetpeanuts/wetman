#include <wetman/utils/test/macro.h>

#include <wetman/utils/mem/memory_tracker_reader.h>

#include "it_healthcheck.c"
#include "it_workspace.c"
#include "it_task.c"
#include "it_mem_out.c"

// Tmp dir macros work only in the context of the test case,
// so we need to wrap it into a separate test
static pid_t __serverPid;
static char  __memOutPath[512];

TEST(IntegrationTest_RunServer)
{
    char wdir[256];
    CREATE_TMP_DIR(wdir);

    char wdirAbs[256];
    GetAbsolutePath(wdir, wdirAbs, sizeof(wdirAbs));

    char memOutDir[256];
    CREATE_TMP_DIR(memOutDir);

    char memOutPath[512];
    snprintf(memOutPath, sizeof(memOutPath), "%s/mem_events.log", memOutDir);
    GetAbsolutePath(memOutPath, __memOutPath, sizeof(__memOutPath));

    char serverCmd[512];
    snprintf(serverCmd, sizeof(serverCmd),
            "WETMAN_WDIR=%s ../build/wetman_server -m %s",
            wdirAbs, __memOutPath);

    __serverPid = Subprocess_RunCommand(serverCmd);
    ASSERT_NE(__serverPid, -1);

    sleep(1);

    // Check that second server instance cannot be run. It gets its own
    // mem-out file so the lock rejection (not the O_EXCL file) is exercised.
    char secondMemOutRel[512];
    snprintf(secondMemOutRel, sizeof(secondMemOutRel),
            "%s/mem_events_second.log", memOutDir);
    char secondMemOutAbs[512];
    GetAbsolutePath(secondMemOutRel, secondMemOutAbs, sizeof(secondMemOutAbs));

    char secondServerCmd[512];
    snprintf(secondServerCmd, sizeof(secondServerCmd),
            "WETMAN_WDIR=%s ../build/wetman_server -m %s",
            wdirAbs, secondMemOutAbs);

    pid_t secondServerPid = Subprocess_RunCommand(secondServerCmd);
    ASSERT_NE(secondServerPid, -1);

    i32 statusCode = Subprocess_WaitFor(secondServerPid, 5000);
    ASSERT_EQ(statusCode, 1);
}

TEST(IntegrationTest_ShutDownServer)
{
    ASSERT(Subprocess_Kill(__serverPid));

    // The server must flush its buffered events and exit on SIGTERM
    // instead of being killed mid-shutdown.
    i32 status = Subprocess_WaitFor(__serverPid, 5000);
    ASSERT_NE(status, -1);

#ifdef WETMAN_ENABLE_MEMORY_TRACKER
    // Every arena the server allocated over the whole run must have been
    // freed by the time it shut down.
    Arena verifyArena = Arena_New();
    MemoryEventList list;
    ASSERT_EQ(MemoryTrackerReader_ReadFromFile(
            Str_FromCStr(__memOutPath), &verifyArena, &list), 0);
    ASSERT(MemoryTrackerReader_VerifyAllArenasFreed(&list, &verifyArena));
    Arena_Free(&verifyArena);
#endif // WETMAN_ENABLE_MEMORY_TRACKER
}

void registerIntegrationTests(void)
{
    // Tests without running server
    REGISTER_TEST(IntegrationTest_HealthCheck_NoServerRunning);
    REGISTER_TEST(IntegrationTest_Workspace_NoServerRunning);

    // Tests spawning their own short-lived server instance
    REGISTER_TEST(IntegrationTest_MemOut_CreatesFileAndFlushesEvents);
    REGISTER_TEST(IntegrationTest_MemOut_ExistingFileFails);
    REGISTER_TEST(IntegrationTest_MemOut_InvalidArgs);

    // Tests required server instance running
    REGISTER_TEST(IntegrationTest_RunServer);

    REGISTER_TEST(IntegrationTest_HealthCheck_Success);
    REGISTER_TEST(IntegrationTest_Workspace_InitDelete_DefaultArgs);
    REGISTER_TEST(IntegrationTest_Workspace_InitDelete_NamedArgs);
    REGISTER_TEST(IntegrationTest_Task_Get);
    REGISTER_TEST(IntegrationTest_Task_List);
    REGISTER_TEST(IntegrationTest_Task_List_Empty);
    REGISTER_TEST(IntegrationTest_Task_Edit);
    REGISTER_TEST(IntegrationTest_Task_Print);
    REGISTER_TEST(IntegrationTest_Task_Delete);

    REGISTER_TEST(IntegrationTest_ShutDownServer);
}
