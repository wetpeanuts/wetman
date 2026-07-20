#ifndef WETMAN_TEST_UTILS_TEST_MANAGER_H
#define WETMAN_TEST_UTILS_TEST_MANAGER_H

#include <stdint.h>


#define MAX_TEST_COUNT 1024

typedef struct {
    uint32_t failedExpectCount;
    uint32_t failedAssertCount;
} TestCaseStat;

typedef struct {
    void(*tests[MAX_TEST_COUNT])(TestCaseStat*);
    const char* testNames[MAX_TEST_COUNT];
    uint32_t testCount;
    uint32_t testRunCount;
    uint32_t failedTestCount;
} TestManager;

void TestManager_RegisterTest(void(*test)(TestCaseStat*), const char* testName);
void TestManager_RunTests(void);

extern TestManager globalTestManager;

#endif // WETMAN_TEST_UTILS_TEST_MANAGER_H 
