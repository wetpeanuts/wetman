#include <wetman/utils/test/macro.h>

#include <wetman/utils/mod.c>
#include "utils/mod.c"


int main(void)
{
    registerUtilTests();

    RUN_TESTS_IN_WDIR(".test_wdir");
}
