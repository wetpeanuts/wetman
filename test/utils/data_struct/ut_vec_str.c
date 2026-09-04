#include <wetman/utils/data_struct/vec_str.h>
#include <wetman/utils/test/macro.h>


TEST(VecStrTest_New)
{
    Arena arena = Arena_New();
    VecStr vec = VecStr_New(&arena);

    EXPECT_EQ(vec.len, 0);
    EXPECT_EQ(vec.cap, 2);

    Arena_Free(&arena);
}

TEST(VecStrTest_WithCapacity)
{
    Arena arena = Arena_New();
    VecStr vec = VecStr_WithCapacity(5, &arena);

    EXPECT_EQ(vec.len, 0);
    EXPECT_EQ(vec.cap, 5);

    Arena_Free(&arena);
}

TEST(VecStrTest_PushAt)
{
    Arena arena = Arena_New();
    VecStr vec = VecStr_WithCapacity(2, &arena);

    Str v1 = Str_FromCStr("hello");
    Str v2 = Str_FromCStr("world");
    Str v3 = Str_FromCStr("!");

    ASSERT(VecStr_Push(&vec, &v1));
    EXPECT_EQ(vec.len, 1);
    EXPECT(Str_EqCStr(*VecStr_At(&vec, 0), "hello"));

    ASSERT(VecStr_Push(&vec, &v2));
    EXPECT_EQ(vec.len, 2);
    EXPECT(Str_EqCStr(*VecStr_At(&vec, 0), "hello"));
    EXPECT(Str_EqCStr(*VecStr_At(&vec, 1), "world"));

    ASSERT(VecStr_Push(&vec, &v3));
    EXPECT_EQ(vec.len, 3);
    EXPECT(Str_EqCStr(*VecStr_At(&vec, 0), "hello"));
    EXPECT(Str_EqCStr(*VecStr_At(&vec, 1), "world"));
    EXPECT(Str_EqCStr(*VecStr_At(&vec, 2), "!"));

    Arena_Free(&arena);
}

TEST(VecStrTest_AtOutOfRange)
{
    Arena arena = Arena_New();
    VecStr vec = VecStr_New(&arena);

    EXPECT_EQ(vec.len, 0);
    EXPECT(VecStr_At(&vec, 0) == NULL);
    EXPECT(VecStr_At(&vec, 5) == NULL);

    Str v1 = Str_FromCStr("test");
    ASSERT(VecStr_Push(&vec, &v1));
    EXPECT(VecStr_At(&vec, 1) == NULL);
    EXPECT(VecStr_At(&vec, 100) == NULL);

    Arena_Free(&arena);
}
