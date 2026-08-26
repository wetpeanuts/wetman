#include <wetman/utils/data_struct/vec.h>
#include <wetman/utils/test/macro.h>


TEST(VecTest_New)
{
    Arena arena = Arena_New();
    Vec vec = VEC_NEW(i32, &arena);

    EXPECT_EQ(vec.len, 0);
    EXPECT_EQ(vec.cap, 2);

    Arena_Free(&arena);
}

TEST(VecTest_WithCapacity)
{
    Arena arena = Arena_New();
    Vec vec = VEC_WITH_CAPACITY(i32, 5, &arena);

    EXPECT_EQ(vec.len, 0);
    EXPECT_EQ(vec.cap, 5);

    Arena_Free(&arena);
}

TEST(VecTest_PushAt)
{
    Arena arena = Arena_New();
    Vec vec = VEC_WITH_CAPACITY(i32, 2, &arena);

    i32 v1 = 10;
    i32 v2 = 20;
    i32 v3 = 30;

    ASSERT(VEC_PUSH(i32, &vec, &v1));
    EXPECT_EQ(vec.len, 1);
    EXPECT_EQ(*VEC_AT(i32, &vec, 0), 10);

    ASSERT(VEC_PUSH(i32, &vec, &v2));
    EXPECT_EQ(vec.len, 2);
    EXPECT_EQ(*VEC_AT(i32, &vec, 0), 10);
    EXPECT_EQ(*VEC_AT(i32, &vec, 1), 20);

    ASSERT(VEC_PUSH(i32, &vec, &v3));
    EXPECT_EQ(vec.len, 3);
    EXPECT_EQ(*VEC_AT(i32, &vec, 0), 10);
    EXPECT_EQ(*VEC_AT(i32, &vec, 1), 20);
    EXPECT_EQ(*VEC_AT(i32, &vec, 2), 30);

    Arena_Free(&arena);
}

TEST(VecTest_AtOutOfRange)
{
    Arena arena = Arena_New();
    Vec vec = VEC_NEW(i32, &arena);

    EXPECT_EQ(vec.len, 0);
    EXPECT(VEC_AT(i32, &vec, 0) == NULL);
    EXPECT(VEC_AT(i32, &vec, 5) == NULL);

    i32 v1 = 42;
    ASSERT(VEC_PUSH(i32, &vec, &v1));
    EXPECT(VEC_AT(i32, &vec, 1) == NULL);
    EXPECT(VEC_AT(i32, &vec, 100) == NULL);

    Arena_Free(&arena);
}
