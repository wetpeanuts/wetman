#ifndef WETMAN_TEST_UTILS_MACRO_H
#define WETMAN_TEST_UTILS_MACRO_H

#include <wetman/utils/macro.h>
#include <wetman/utils/test/test_manager.h>

#include <stdio.h>


#define TEST(test) void test( \
        __TestCaseContext*          __testCaseContext          MAYBE_UNUSED, \
        __TestManagerGlobalContext* __testManagerGlobalContext MAYBE_UNUSED)

#define REGISTER_TEST(test) __TestManager_RegisterTest(test, #test)

#define RUN_TESTS() __TestManager_RunTests(".")
#define RUN_TESTS_IN_WDIR(wdir) __TestManager_RunTests(wdir)

#define CREATE_TMP_FILE(flags) (__TestManager_CreateTmpFile(__testManagerGlobalContext, __testCaseContext, flags))
#define CREATE_TMP_DIR(tmpPathDir) (__TestManager_CreateTmpDir(__testManagerGlobalContext, __testCaseContext, tmpPathDir))

#define EXPECT_EQ(actual, expected) \
    do { \
        if ((actual) != (expected)) { \
            printf("[FAIL] %s:%d\n", __FILE__, __LINE__); \
            printf("  Expected: %lld (%s)\n", (long long)(expected), (#expected)); \
            printf("  Actual:   %lld (%s)\n", (long long)(actual), (#actual)); \
            __testCaseContext->failureCount++; \
        } \
    } while (0)

#define EXPECT_NE(actual, expected) \
    do { \
        if ((actual) == (expected)) { \
            printf("[FAIL] %s:%d\n", __FILE__, __LINE__); \
            printf("  Expected: %s != %s (%lld)\n", (#actual), (#expected), (long long)(actual)); \
            __testCaseContext->failureCount++; \
        } \
    } while (0)

#define EXPECT(condition) \
    do { \
        if (!(condition)) { \
            printf("[FAIL] %s:%d\n", __FILE__, __LINE__); \
            printf("  Condition is false: %s\n", (#condition)); \
            __testCaseContext->failureCount++; \
        } \
    } while (0)

#define ASSERT_EQ(actual, expected) \
    do { \
        if ((actual) != (expected)) { \
            printf("[FAIL] %s:%d\n", __FILE__, __LINE__); \
            printf("  Expected: %lld (%s)\n", (long long)(expected), (#expected)); \
            printf("  Actual:   %lld (%s)\n", (long long)(actual), (#actual)); \
            __testCaseContext->failureCount++; \
            return; \
        } \
    } while (0)

#define ASSERT_NE(actual, expected) \
    do { \
        if ((actual) == (expected)) { \
            printf("[FAIL] %s:%d\n", __FILE__, __LINE__); \
            printf("  Expected: %s != %s (%lld)\n", (#actual), (#expected), (long long)(actual)); \
            __testCaseContext->failureCount++; \
            return; \
        } \
    } while (0)

#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            printf("[FAIL] %s:%d\n", __FILE__, __LINE__); \
            printf("  Condition is false: %s\n", (#condition)); \
            __testCaseContext->failureCount++; \
            return; \
        } \
    } while (0)

#endif // WETMAN_TEST_UTILS_MACRO_H 
