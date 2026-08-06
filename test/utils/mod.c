#include <wetman/utils/test/macro.h>

#include "ut_data_stream.c"

#include "async/mod.c"
#include "data_struct/mod.c"
#include "mem/mod.c"
#include "net/mod.c"
#include "proc/mod.c"


void registerUtilTests(void)
{
    REGISTER_TEST(DataStream_PushPop_I32);
    REGISTER_TEST(DataStream_PushPop_U32);
    REGISTER_TEST(DataStream_PushPop_Str);

    registerUtilAsyncTests();
    registerUtilDataStructTests();
    registerUtilMemTests();
    registerUtilNetTests();
    registerUtilProcTests();
}
