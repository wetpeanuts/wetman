#ifndef WETMAN_TEST_UTILS_TEST_MANAGER_H
#define WETMAN_TEST_UTILS_TEST_MANAGER_H

#include <wetman/utils/type.h>

#include <stdint.h>


#define MAX_TEST_COUNT 1024

typedef struct {
    const char* cwd;
    const char* tmpFileDir;
    const char* tmpDirDir;
    u32         tmpFileCount;
    u32         tmpDirCount;
} __TestManagerGlobalContext;

typedef struct {
    const char* testCaseName;
    u32         failureCount;
} __TestCaseContext;

typedef void(*__TestCaseHandler)(__TestCaseContext*, __TestManagerGlobalContext*);

typedef struct {
    __TestManagerGlobalContext context;
    __TestCaseHandler          tests[MAX_TEST_COUNT];
    const char*                testNames[MAX_TEST_COUNT];
    u32                        testCount;
    u32                        testRunCount;
    u32                        failedTestCount;
    i64                        totalTime;
} __TestManager;

void __TestManager_RegisterTest(__TestCaseHandler test, const char* testName);
void __TestManager_RunTests(const char* wdir);
i32  __TestManager_CreateTmpFile(
        __TestManagerGlobalContext* globalContext,
        __TestCaseContext*          testCaseContext,
        i32                         flags);

// Writes null terminated path to temporary dir into tmpDirPath
void __TestManager_CreateTmpDir(
        __TestManagerGlobalContext* globalContext,
        __TestCaseContext*          testCaseContext,
        char*                       tmpDirPath);

extern __TestManager __globalTestManager;

#endif // WETMAN_TEST_UTILS_TEST_MANAGER_H 
