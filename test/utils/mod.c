#include <wetman/test/utils/macro.h>

#include "ut_str.c"

void registerUtilTests(void)
{
    REGISTER_TEST(StrTest_EqCStr);
    REGISTER_TEST(StrTest_EqStr);
    REGISTER_TEST(StrTest_FromCStr_EmptyStr);
    REGISTER_TEST(StrTest_FromCStr_NonEmptyStr);
}
