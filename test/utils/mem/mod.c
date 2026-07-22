#include <wetman/utils/test/macro.h>

#include "ut_arena.c"

void registerUtilMemTests(void)
{
    REGISTER_TEST(ArenaTest_New);
    REGISTER_TEST(ArenaTest_Alloc);
    REGISTER_TEST(ArenaTest_Reset);
    REGISTER_TEST(ArenaTest_Free);
}
