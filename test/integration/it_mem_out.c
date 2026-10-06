#include <wetman/utils/proc/subprocess.h>
#include <wetman/utils/test/macro.h>

#include "utils.h"

#include <stdio.h>
#include <string.h>


// Runs wetman_server with the given args (stdout+stderr redirected to outFile)
// and returns its exit status, or -1 on wait timeout.
static i32 RunServerWithOut(const char* args, const char* outFile)
{
    char serverPath[1024];
    GetAbsolutePath("../build/wetman_server", serverPath, sizeof(serverPath));

    char outPath[1024];
    GetAbsolutePath(outFile, outPath, sizeof(outPath));

    char cmd[2048];
    snprintf(cmd, sizeof(cmd), "%s %s > %s 2>&1", serverPath, args, outPath);

    pid_t pid = Subprocess_RunCommand(cmd);
    if (pid == -1) {
        return -1;
    }

    return Subprocess_WaitFor(pid, 5000);
}

// Starts the server with -m pointing at a fresh path, shuts it down via
// SIGTERM (graceful, flushes tracker events) and checks the output file.
TEST(IntegrationTest_MemOut_CreatesFileAndFlushesEvents)
{
    char wdir[256];
    CREATE_TMP_DIR(wdir);

    char wdirAbs[256];
    GetAbsolutePath(wdir, wdirAbs, sizeof(wdirAbs));

    char memOutDir[256];
    CREATE_TMP_DIR(memOutDir);

    char memOutPath[512];
    snprintf(memOutPath, sizeof(memOutPath), "%s/mem.log", memOutDir);

    char memOutAbs[512];
    GetAbsolutePath(memOutPath, memOutAbs, sizeof(memOutAbs));

    char serverCmd[2048];
    snprintf(serverCmd, sizeof(serverCmd),
            "WETMAN_WDIR=%s ../build/wetman_server -m %s",
            wdirAbs, memOutAbs);

    pid_t serverPid = Subprocess_RunCommand(serverCmd);
    ASSERT_NE(serverPid, -1);

    sleep(1);

    ASSERT(Subprocess_Kill(serverPid));
    i32 status = Subprocess_WaitFor(serverPid, 5000);
    ASSERT_NE(status, -1);

    char buf[8192];
    ASSERT_EQ(ReadFile(memOutAbs, buf, sizeof(buf)), 0);
    Str out = Str_FromCStr(buf);
    EXPECT(Str_Contains(out, Str_FromCStr("MemoryEvent")) >= 0);
    EXPECT(Str_Contains(out, Str_FromCStr("ARENA_CREATED")) >= 0);
}

// The long form must be recognized: an existing file is an error
// ("cannot create..."), not "Unknown option", and stays untouched.
TEST(IntegrationTest_MemOut_ExistingFileFails)
{
    char dir[256];
    CREATE_TMP_DIR(dir);

    char memOutPath[512];
    snprintf(memOutPath, sizeof(memOutPath), "%s/mem.log", dir);

    FILE* memOutFile = fopen(memOutPath, "w");
    ASSERT(memOutFile != NULL);
    fputs("sentinel\n", memOutFile);
    fclose(memOutFile);

    char memOutAbs[512];
    GetAbsolutePath(memOutPath, memOutAbs, sizeof(memOutAbs));

    char args[1024];
    snprintf(args, sizeof(args), "--mem-out %s", memOutAbs);

    char outPath[512];
    snprintf(outPath, sizeof(outPath), "%s/server_out.txt", dir);

    EXPECT_EQ(RunServerWithOut(args, outPath), 1);

    char outBuf[2048];
    ASSERT_EQ(ReadFile(outPath, outBuf, sizeof(outBuf)), 0);
    Str out = Str_FromCStr(outBuf);
    EXPECT(Str_Contains(out, Str_FromCStr("cannot create memory output file")) >= 0);

    char memOutBuf[256];
    ASSERT_EQ(ReadFile(memOutAbs, memOutBuf, sizeof(memOutBuf)), 0);
    EXPECT(strcmp(memOutBuf, "sentinel\n") == 0);
}

TEST(IntegrationTest_MemOut_InvalidArgs)
{
    char dir[256];
    CREATE_TMP_DIR(dir);

    // Missing value for -m
    char missingValueOut[512];
    snprintf(missingValueOut, sizeof(missingValueOut),
            "%s/missing_value.txt", dir);
    EXPECT_EQ(RunServerWithOut("-m", missingValueOut), 1);

    char outBuf[2048];
    ASSERT_EQ(ReadFile(missingValueOut, outBuf, sizeof(outBuf)), 0);
    EXPECT(Str_Contains(Str_FromCStr(outBuf),
                Str_FromCStr("Missing value for option")) >= 0);

    // Unknown option is rejected before the server starts
    char unknownOut[512];
    snprintf(unknownOut, sizeof(unknownOut), "%s/unknown_option.txt", dir);
    EXPECT_EQ(RunServerWithOut("--unknown", unknownOut), 1);

    ASSERT_EQ(ReadFile(unknownOut, outBuf, sizeof(outBuf)), 0);
    EXPECT(Str_Contains(Str_FromCStr(outBuf),
                Str_FromCStr("Unknown option")) >= 0);
}
