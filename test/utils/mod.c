#include <wetman/utils/test/macro.h>

#include "data_struct/mod.c"
#include "mem/mod.c"
#include "net/mod.c"

void registerUtilTests(void)
{
    registerUtilDataStructTests();
    registerUtilMemTests();
    registerUtilNetTests();
}
