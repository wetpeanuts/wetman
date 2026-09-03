#include <wetman/utils/test/macro.h>

#include "ut_data_stream.c"

#include "async/mod.c"
#include "data_struct/mod.c"
#include "mem/mod.c"
#include "net/mod.c"
#include "proc/mod.c"


void registerUtilTests(void)
{
    REGISTER_TEST(DataStreamTest_PushPop_I32);
    REGISTER_TEST(DataStreamTest_PushPop_U32);
    REGISTER_TEST(DataStreamTest_PushPop_U64);
    REGISTER_TEST(DataStreamTest_PushPop_Str);
    REGISTER_TEST(DataStreamTest_ReadWrite);
    REGISTER_TEST(DataStreamTest_MultipleRead);
    REGISTER_TEST(DataStreamTest_Append);
    REGISTER_TEST(DataStreamTest_PushPop_SliceI32);
    REGISTER_TEST(DataStreamTest_PushPop_SliceI32_Empty);
    REGISTER_TEST(DataStreamTest_PushPop_SliceStr);
    REGISTER_TEST(DataStreamTest_PushPop_SliceStr_Empty);
    REGISTER_TEST(DataStreamTest_PushPop_SliceU32);
    REGISTER_TEST(DataStreamTest_PushPop_SliceU32_Empty);
    REGISTER_TEST(DataStreamTest_PushPop_SliceU64);
    REGISTER_TEST(DataStreamTest_PushPop_SliceU64_Empty);
    REGISTER_TEST(DataStreamTest_PushPop_SliceI64);
    REGISTER_TEST(DataStreamTest_PushPop_SliceI64_Empty);

    registerUtilAsyncTests();
    registerUtilDataStructTests();
    registerUtilMemTests();
    registerUtilNetTests();
    registerUtilProcTests();
}
