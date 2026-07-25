#include <wetman/utils/test/macro.h>

#include "ut_arena.c"

void registerUtilMemTests(void)
{
    REGISTER_TEST(ArenaTest_NewFree);
    REGISTER_TEST(ArenaTest_NewReset);
    REGISTER_TEST(ArenaTest_Alloc);
    REGISTER_TEST(ArenaTest_CanAllocOnSamePage);
}
