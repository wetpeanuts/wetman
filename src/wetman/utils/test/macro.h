#ifndef WETMAN_TEST_UTILS_MACRO_H
#define WETMAN_TEST_UTILS_MACRO_H

#include <wetman/utils/macro.h>
#include <wetman/utils/test/test_manager.h>

#include <stdio.h>


#define TEST(test) void test(TestCaseStat* __testCaseStat MAYBE_UNUSED)

#define REGISTER_TEST(test) TestManager_RegisterTest(test, #test)

#define RUN_TESTS() TestManager_RunTests()

#define EXPECT_EQ(actual, expected) \
    do { \
        if ((expected) != (actual)) { \
            printf("[FAIL] %s:%d\n", __FILE__, __LINE__); \
            printf("  Expected: %lld (%s)\n", (long long)(expected), (#expected)); \
            printf("  Actual:   %lld (%s)\n", (long long)(actual), (#actual)); \
            __testCaseStat->failureCount++; \
        } \
    } while (0)

#define EXPECT_NE(actual, expected) \
    do { \
        if ((expected) == (actual)) { \
            printf("[FAIL] %s:%d\n", __FILE__, __LINE__); \
            printf("  Expected: %s != %s (%lld)\n", (#actual), (#expected), (long long)(actual)); \
            __testCaseStat->failureCount++; \
        } \
    } while (0)

#define EXPECT(condition) \
    do { \
        if (!(condition)) { \
            printf("[FAIL] %s:%d\n", __FILE__, __LINE__); \
            printf("  Condition is false: %s\n", (#condition)); \
            __testCaseStat->failureCount++; \
        } \
    } while (0)

#define ASSERT_EQ(actual, expected) \
    do { \
        if ((expected) != (actual)) { \
            printf("[FAIL] %s:%d\n", __FILE__, __LINE__); \
            printf("  Expected: %lld (%s)\n", (long long)(expected), (#expected)); \
            printf("  Actual:   %lld (%s)\n", (long long)(actual), (#actual)); \
            __testCaseStat->failureCount++; \
            return; \
        } \
    } while (0)

#define ASSERT_NE(actual, expected) \
    do { \
        if ((expected) == (actual)) { \
            printf("[FAIL] %s:%d\n", __FILE__, __LINE__); \
            printf("  Expected: %s != %s (%lld)\n", (#actual), (#expected), (long long)(actual)); \
            __testCaseStat->failureCount++; \
            return; \
        } \
    } while (0)

#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            printf("[FAIL] %s:%d\n", __FILE__, __LINE__); \
            printf("  Condition is false: %s\n", (#condition)); \
            __testCaseStat->failureCount++; \
            return; \
        } \
    } while (0)

#endif // WETMAN_TEST_UTILS_MACRO_H 
