#include <wetman/utils/data_struct/vec_i32.h>
#include <wetman/utils/test/macro.h>


TEST(VecI32Test_New)
{
    Arena arena = Arena_New();
    VecI32 vec = VecI32_New(&arena);

    EXPECT_EQ(vec.len, 0);
    EXPECT_EQ(vec.cap, 2);

    Arena_Free(&arena);
}

TEST(VecI32Test_WithCapacity)
{
    Arena arena = Arena_New();
    VecI32 vec = VecI32_WithCapacity(5, &arena);

    EXPECT_EQ(vec.len, 0);
    EXPECT_EQ(vec.cap, 5);

    Arena_Free(&arena);
}

TEST(VecI32Test_PushAt)
{
    Arena arena = Arena_New();
    VecI32 vec = VecI32_WithCapacity(2, &arena);

    i32 v1 = 10;
    i32 v2 = 20;
    i32 v3 = 30;

    ASSERT(VecI32_Push(&vec, &v1));
    EXPECT_EQ(vec.len, 1);
    EXPECT_EQ(*VecI32_At(&vec, 0), 10);

    ASSERT(VecI32_Push(&vec, &v2));
    EXPECT_EQ(vec.len, 2);
    EXPECT_EQ(*VecI32_At(&vec, 0), 10);
    EXPECT_EQ(*VecI32_At(&vec, 1), 20);

    ASSERT(VecI32_Push(&vec, &v3));
    EXPECT_EQ(vec.len, 3);
    EXPECT_EQ(*VecI32_At(&vec, 0), 10);
    EXPECT_EQ(*VecI32_At(&vec, 1), 20);
    EXPECT_EQ(*VecI32_At(&vec, 2), 30);

    Arena_Free(&arena);
}

TEST(VecI32Test_AtOutOfRange)
{
    Arena arena = Arena_New();
    VecI32 vec = VecI32_New(&arena);

    EXPECT_EQ(vec.len, 0);
    EXPECT(VecI32_At(&vec, 0) == NULL);
    EXPECT(VecI32_At(&vec, 5) == NULL);

    i32 v1 = 42;
    ASSERT(VecI32_Push(&vec, &v1));
    EXPECT(VecI32_At(&vec, 1) == NULL);
    EXPECT(VecI32_At(&vec, 100) == NULL);

    Arena_Free(&arena);
}
