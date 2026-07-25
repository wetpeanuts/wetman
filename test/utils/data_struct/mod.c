#include <wetman/utils/test/macro.h>

#include "ut_str.c"

void registerUtilDataStructTests(void)
{
    REGISTER_TEST(StrTest_EqCStr);
    REGISTER_TEST(StrTest_EqStr);
    REGISTER_TEST(StrTest_FromCStr);
    REGISTER_TEST(StrTest_Concat);
}
