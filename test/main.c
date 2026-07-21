#include <wetman/test/utils/macro.h>

#include <wetman/test/mod.c>
#include "utils/mod.c"


int main(void)
{
    registerUtilTests();

    RUN_TESTS();
}
