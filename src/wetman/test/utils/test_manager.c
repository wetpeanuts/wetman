#include <wetman/test/utils/test_manager.h>

#include <stdio.h>
#include <stdlib.h>

const char* resultLabel(int success)
{
    if (success) {
        return "\033[32mSUCCESS\033[0m";
    } else {
        return "\033[31mFAILED\033[0m";
    }
}

void TestManager_RegisterTest(void(*test)(TestCaseStat*), const char* testName)
{
    if (__globalTestManager.testCount >= MAX_TEST_COUNT) {
        printf("Error: max test count (%d) exceeded. Consider updaing max test count.", MAX_TEST_COUNT);
        exit(EXIT_FAILURE);
    }
    __globalTestManager.tests[__globalTestManager.testCount] = test;
    __globalTestManager.testNames[__globalTestManager.testCount++] = testName;
}

void TestManager_RunTests(void)
{
    for (uint32_t i = 0; i < __globalTestManager.testCount; ++i) {
        const uint32_t testCount = __globalTestManager.testCount;
        const char* testName = __globalTestManager.testNames[i];

        // TODO: align test name column
        printf("[RUNNING: %d/%d] %s\n", i + 1, testCount, testName);

        __globalTestManager.testRunCount++;
        TestCaseStat testCaseStat = {
            .failureCount = 0,
        };
        __globalTestManager.tests[i](&testCaseStat);
        const char* result = resultLabel(!testCaseStat.failureCount);
        if (testCaseStat.failureCount) {
            __globalTestManager.failedTestCount++;
        }

        printf("[...DONE: %s] %s\n", result, testName);
    }

    const uint32_t testCount = __globalTestManager.testCount;
    const uint32_t testRunCount = __globalTestManager.testRunCount;
    const uint32_t failedTestCount = __globalTestManager.failedTestCount;

    const uint32_t successTestCount = testRunCount - failedTestCount;
    const uint32_t skippedTestCount = testCount - testRunCount;

    const char* result = resultLabel(!skippedTestCount && !failedTestCount);

    printf("========\n");
    printf("[RESULT: %s]\n", result);
    printf("========\n");
    printf("[SUCCEED: \033[32m%d/%d\033[0m]\n", successTestCount, testCount);
    printf("[SKIPPED: \033[31m%d/%d\033[0m]\n", skippedTestCount, testCount);
    printf("[FAILED:  \033[31m%d/%d\033[0m]\n", failedTestCount, testCount);
}

TestManager __globalTestManager = {
    .testCount = 0,
    .testRunCount = 0,
    .failedTestCount = 0,
};

