#include <wetman/utils/test/test_manager.h>

#include <wetman/utils/time.h>

#include <stdio.h>
#include <stdlib.h>

const char* __resultLabel(int success)
{
    if (success) {
        return "\033[32mPASSED\033[0m";
    } else {
        return "\033[31mFAILED\033[0m";
    }
}

void __timeToStr(char* buf, size_t bufLen, long long ts)
{
    if (ts > 1000) {
        snprintf(buf, bufLen, "%llds", ts / 1000);
    } else {
        snprintf(buf, bufLen, "%lldms", ts);
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
    char statusBuf[256];
    const size_t statusBufLen = sizeof(statusBuf);
    char timeBuf[32];
    const size_t timeBufLen = sizeof(timeBuf);
    const int prefixlen = 24;
    const char* gap = "........................";


    const long long totalTsBegin = currentTimestampMs();
    for (uint32_t i = 0; i < __globalTestManager.testCount; ++i) {
        const uint32_t testCount = __globalTestManager.testCount;
        const char* testName = __globalTestManager.testNames[i];

        int actualPrefixLen = snprintf(statusBuf, statusBufLen, "[RUNNING: %d/%d]", i + 1, testCount);
        printf("%s%.*s%s\n", statusBuf, prefixlen - actualPrefixLen, gap, testName);

        __globalTestManager.testRunCount++;
        TestCaseStat testCaseStat = {
            .failureCount = 0,
        };

        const long long tsBegin = currentTimestampMs();
        __globalTestManager.tests[i](&testCaseStat);
        const long long tsEnd = currentTimestampMs();

        const long long testTime = tsEnd - tsBegin;
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
    const long long totalTsEnd = currentTimestampMs();
    const long long totalTime = totalTsEnd - totalTsBegin;
    __timeToStr(timeBuf, timeBufLen, totalTime);

    const uint32_t testCount = __globalTestManager.testCount;
    const uint32_t testRunCount = __globalTestManager.testRunCount;
    const uint32_t failedTestCount = __globalTestManager.failedTestCount;

    const uint32_t successTestCount = testRunCount - failedTestCount;
    const uint32_t skippedTestCount = testCount - testRunCount;

    const char* result = __resultLabel(!skippedTestCount && !failedTestCount);

    printf("=================\n");
    printf("[RESULT:  %s] Total time: %s\n", result, timeBuf);
    printf("=================\n");
    printf("[PASSED:  \033[32m%d/%d\033[0m]\n", successTestCount, testCount);
    printf("[SKIPPED: \033[31m%d/%d\033[0m]\n", skippedTestCount, testCount);
    printf("[FAILED:  \033[31m%d/%d\033[0m]\n", failedTestCount, testCount);
}

TestManager __globalTestManager = {
    .testCount = 0,
    .testRunCount = 0,
    .failedTestCount = 0,
};

