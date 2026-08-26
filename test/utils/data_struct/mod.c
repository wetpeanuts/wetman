#include <wetman/utils/test/macro.h>

#include "ut_str.c"
#include "ut_vec.c"

void registerUtilDataStructTests(void)
{
    REGISTER_TEST(StrTest_EqCStr);
    REGISTER_TEST(StrTest_EqStr);
    REGISTER_TEST(StrTest_FromCStr);
    REGISTER_TEST(StrTest_FromU64);
    REGISTER_TEST(StrTest_Concat);

    REGISTER_TEST(VecTest_New);
    REGISTER_TEST(VecTest_WithCapacity);
    REGISTER_TEST(VecTest_PushAt);
    REGISTER_TEST(VecTest_AtOutOfRange);
}
