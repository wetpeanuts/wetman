#include <wetman/utils/test/test_manager.h>

#include <wetman/utils/filesystem.h>
#include <wetman/utils/time.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

const char* __resultLabel(i32 success)
{
    if (success) {
        return "\033[32mPASSED\033[0m";
    } else {
        return "\033[31mFAILED\033[0m";
    }
}

void __timeToStr(char* buf, usize bufLen, i64 ts)
{
    if (ts > 1000) {
        snprintf(buf, bufLen, "%llds", ts / 1000);
    } else {
        snprintf(buf, bufLen, "%lldms", ts);
    }
}

void __setupWorkingDirectory(const char* wdir)
{
    if (strcmp(wdir, ".") != 0) {
        if (mkdir(wdir, 0755) == -1 && errno != EEXIST) {
            printf("Error: failed to create test working directory");
            exit(1);
        }
        if (chdir(wdir) == -1) {
            printf("Error: failed to change working directory");
            exit(1);
        }
    }

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("Error: failed to get current working directory");
        exit(1);
    }

    __globalTestManager.context.cwd = cwd;
    printf("Test working directory: %s\n", cwd);
}

void __setupTmpDir(const char* path)
{
    if (FS_RemoveDirectoryRecursive(path) != 0) {
        printf("Error: failed to init tmp file directory");
        exit(1);
    }
    if (mkdir(path, 0755) == -1) {
        printf("Error: failed to init tmp file directory");
        exit(1);
    }
}

void __cleanTestWorkspace(void)
{
    if (FS_RemoveDirectoryRecursive(__globalTestManager.context.tmpFileDir) != 0
            || FS_RemoveDirectoryRecursive(__globalTestManager.context.tmpDirDir) != 0) {
        printf("Error: failed to clean test workspace");
        exit(1);
    }
}

void __printTestStat(void)
{
    char        timeBuf[32];
    const usize timeBufLen = sizeof(timeBuf);

    const u32 testCount       = __globalTestManager.testCount;
    const u32 testRunCount    = __globalTestManager.testRunCount;
    const u32 failedTestCount = __globalTestManager.failedTestCount;
    const i64 totalTime       = __globalTestManager.totalTime;
    __timeToStr(timeBuf, timeBufLen, totalTime);

    const u32 successTestCount = testRunCount - failedTestCount;
    const u32 skippedTestCount = testCount - testRunCount;

    const char* result = __resultLabel(!skippedTestCount && !failedTestCount);

    printf("=================\n");
    printf("[RESULT:  %s] Total time: %s\n", result, timeBuf);
    printf("=================\n");
    printf("[PASSED:  \033[32m%d/%d\033[0m]\n", successTestCount, testCount);
    printf("[SKIPPED: \033[31m%d/%d\033[0m]\n", skippedTestCount, testCount);
    printf("[FAILED:  \033[31m%d/%d\033[0m]\n", failedTestCount, testCount);
}

void __TestManager_RegisterTest(__TestCaseHandler test, const char* testName)
{
    if (__globalTestManager.testCount >= MAX_TEST_COUNT) {
        printf("Error: max test count (%d) exceeded. Consider updaing max test count.", MAX_TEST_COUNT);
        exit(EXIT_FAILURE);
    }
    __globalTestManager.tests[__globalTestManager.testCount] = test;
    __globalTestManager.testNames[__globalTestManager.testCount++] = testName;
}

void __TestManager_RunTests(const char* wdir)
{
    __setupWorkingDirectory(wdir);
    __setupTmpDir(__globalTestManager.context.tmpFileDir);
    __setupTmpDir(__globalTestManager.context.tmpDirDir);

    char        statusBuf[256];
    const usize statusBufLen = sizeof(statusBuf);
    char        timeBuf[32];
    const usize timeBufLen = sizeof(timeBuf);
    const i32   prefixlen = 24;
    const char* gap = "........................";


    const i64 totalTsBegin = currentTimestampMs();
    for (u32 i = 0; i < __globalTestManager.testCount; ++i) {
        const u32   testCount = __globalTestManager.testCount;
        const char* testName  = __globalTestManager.testNames[i];

        i32 actualPrefixLen = snprintf(statusBuf, statusBufLen, "[RUNNING: %d/%d]", i + 1, testCount);
        printf("%s%.*s%s\n", statusBuf, prefixlen - actualPrefixLen, gap, testName);

        __globalTestManager.testRunCount++;
        __TestCaseContext testCaseStat = {
            .testCaseName = testName,
            .failureCount = 0,
        };

        const i64 tsBegin = currentTimestampMs();
        __globalTestManager.tests[i](&testCaseStat, &__globalTestManager.context);
        const i64 tsEnd = currentTimestampMs();

        const i64 testTime = tsEnd - tsBegin;
        __timeToStr(timeBuf, timeBufLen, testTime);

        const char* result = __resultLabel(!testCaseStat.failureCount);
        if (testCaseStat.failureCount) {
            __globalTestManager.failedTestCount++;
        }

        actualPrefixLen = snprintf(statusBuf, statusBufLen, "[...DONE: %s]", result);
        // Correct gap width +9 because of color formatting
        printf("%s%.*s%s %s\n",
                statusBuf, prefixlen - actualPrefixLen + 9, gap, testName, timeBuf);
    }
    const i64 totalTsEnd = currentTimestampMs();
    __globalTestManager.totalTime = totalTsEnd - totalTsBegin;

    __printTestStat();
    // Allow examine workspace after tests are finished
    // TODO: enable cleanup or allow configure cleanup behavior
    // __cleanTestWorkspace();
}

i32  __TestManager_CreateTmpFile(
        __TestManagerGlobalContext* globalContext,
        __TestCaseContext*          testCaseContext,
        i32                         flags)
{
    char        buf[256];
    const usize bufLen = sizeof(buf);

    snprintf(buf, bufLen, "%s/%s_tmp_%d",
            globalContext->tmpFileDir,
            testCaseContext->testCaseName,
            globalContext->tmpFileCount++);

    return open(buf, flags, 0644);
}

void __TestManager_CreateTmpDir(
        __TestManagerGlobalContext* globalContext,
        __TestCaseContext*          testCaseContext,
        char*                       tmpDirPath)
{
    snprintf(tmpDirPath, 256, "%s/%s_tmp_%d",
            globalContext->tmpDirDir,
            testCaseContext->testCaseName,
            globalContext->tmpDirCount++);

    if (mkdir(tmpDirPath, 0755) == -1) {
        printf("Error: failed to create tmp directory");
        exit(1);
    }
}

__TestManager __globalTestManager = {
    .context         = {
        .tmpFileDir   = "tmp_files",
        .tmpDirDir    = "tmp_dirs",
        .tmpFileCount = 0,
        .tmpDirCount  = 0,
    },
    .testCount       = 0,
    .testRunCount    = 0,
    .failedTestCount = 0,
    .totalTime       = 0.0,
};

