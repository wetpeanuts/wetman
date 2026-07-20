#ifndef WETMAN_TEST_UTILS_MACRO_H
#define WETMAN_TEST_UTILS_MACRO_H

#include <wetman/test/utils/test_manager.h>

#include <stdio.h>


#define MAYBE_UNUSED __attribute__((unused))

#define TEST(test) void test(TestCaseStat* __testCaseStat MAYBE_UNUSED)

#define REGISTER_TEST(test) TestManager_RegisterTest(test, #test)

#define RUN_TESTS() TestManager_RunTests()

#define RUN_TEST(test)                     \
    do {                                   \
        printf("Running %s...\n", #test);  \
        test();                            \
    } while (0)

#endif // WETMAN_TEST_UTILS_MACRO_H 
