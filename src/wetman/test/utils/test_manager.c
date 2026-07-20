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
    if (globalTestManager.testCount >= MAX_TEST_COUNT) {
        printf("Error: max test count (%d) exceeded. Consider updaing max test count.", MAX_TEST_COUNT);
        exit(EXIT_FAILURE);
    }
    globalTestManager.tests[globalTestManager.testCount] = test;
    globalTestManager.testNames[globalTestManager.testCount++] = testName;
}

void TestManager_RunTests(void)
{
    for (uint32_t i = 0; i < globalTestManager.testCount; ++i) {
        const uint32_t testCount = globalTestManager.testCount;
        const char* testName = globalTestManager.testNames[i];

        printf("[RUNNING %d/%d] %s\n", i, testCount, testName);

        globalTestManager.testRunCount++;
        TestCaseStat testCaseStat = {
            .failedAssertCount = 0,
            .failedExpectCount = 0,
        };
        globalTestManager.tests[i](&testCaseStat);
        const char* result = resultLabel(
                !testCaseStat.failedExpectCount && !testCaseStat.failedAssertCount);

        printf("[DONE] %s: %s\n", testName, result);
    }

    const uint32_t successTestCount =
        globalTestManager.testRunCount - globalTestManager.failedTestCount;
    const uint32_t skippedTestCount =
        globalTestManager.testCount - globalTestManager.testRunCount;
    const uint32_t failedTestCount = globalTestManager.failedTestCount;

    const char* result = resultLabel(!skippedTestCount && !failedTestCount);

    printf("=======\n");
    printf("[RESULT] %s\n", result);
    printf("=======\n");
    printf("TESTS RUN: %d\n", globalTestManager.testRunCount);
    printf("TESTS SKIPPED: %d\n", skippedTestCount);
    printf("TESTS SUCCEED: %d\n", successTestCount);
    printf("TESTS FAILED: %d\n", globalTestManager.failedTestCount);
}

TestManager globalTestManager = {
    .testCount = 0,
    .testRunCount = 0,
    .failedTestCount = 0,
};

