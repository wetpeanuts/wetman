#include <wetman/utils/test/macro.h>

#include "ut_subprocess.c"

void registerUtilProcTests(void)
{
    REGISTER_TEST(SubprocessTest_RunSuccess);
    REGISTER_TEST(SubprocessTest_RunFailure);
}
