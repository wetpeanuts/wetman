#ifndef WETMAN_TEST_UTILS_MACRO_H
#define WETMAN_TEST_UTILS_MACRO_H

#include <wetman/utils/test/test_manager.h>

#include <stdio.h>


#define MAYBE_UNUSED __attribute__((unused))

#define TEST(test) void test(TestCaseStat* __testCaseStat MAYBE_UNUSED)

#define REGISTER_TEST(test) TestManager_RegisterTest(test, #test)

#define RUN_TESTS() TestManager_RunTests()

#define EXPECT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            printf("[FAIL] %s:%d\n", __FILE__, __LINE__); \
            printf("  Expected: %d (%s)\n", (expected), (#expected)); \
            printf("  Actual:   %d (%s)\n", (actual), (#actual)); \
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

#define ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            printf("[FAIL] %s:%d\n", __FILE__, __LINE__); \
            printf("  Expected: %d (%s)\n", (expected), (#expected)); \
            printf("  Actual:   %d (%s)\n", (actual), (#actual)); \
            __testCaseStat->failureCount++; \
            return; \
        }                                            \
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
