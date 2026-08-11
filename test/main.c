#include <wetman/utils/test/macro.h>

#include <wetman/utils/mod.c>
#include <wetman/server/mod.c>
#include "utils/mod.c"
#include "server/mod.c"


int main(void)
{
    registerUtilTests();
    registerServerTests();

    RUN_TESTS_IN_WDIR(".test_wdir");
}
