#include <wetman/utils/mem/arena.h>
#include <wetman/utils/test/macro.h>


TEST(ArenaTest_New)
{
    Arena arena = Arena_New();
    EXPECT_NE(arena.data, NULL);
    EXPECT_EQ(arena.size, 0);
    EXPECT_EQ(arena.capacity, 4096);
}

TEST(ArenaTest_Alloc)
{
    Arena arena = Arena_New();
    void* arenaDataPtr = arena.data;
    ASSERT_NE(arenaDataPtr, NULL);

    void* memPtr = Arena_Alloc(&arena, 1024);
    EXPECT_EQ(arena.data, arenaDataPtr);
    EXPECT_EQ(arena.size, 1024);
    EXPECT_EQ(arena.capacity, 4096);
    EXPECT_EQ(memPtr, arena.data);

    memPtr = Arena_Alloc(&arena, 2048);
    EXPECT_EQ(arena.data, arenaDataPtr);
    EXPECT_EQ(arena.size, 3072);
    EXPECT_EQ(arena.capacity, 4096);
    EXPECT_EQ(memPtr, arena.data + 1024);

    memPtr = Arena_Alloc(&arena, 2048);
    // Might realloc
    arenaDataPtr = arena.data;
    EXPECT_EQ(arena.size, 5120);
    EXPECT_EQ(arena.capacity, 8192);
    EXPECT_EQ(memPtr, arena.data + 3072);

    memPtr = Arena_Alloc(&arena, 3072);
    EXPECT_EQ(arena.data, arenaDataPtr);
    EXPECT_EQ(arena.size, 8192);
    EXPECT_EQ(arena.capacity, 8192);
    EXPECT_EQ(memPtr, arena.data + 5120);
}

TEST(ArenaTest_Reset)
{
    Arena arena = Arena_New();
    void* arenaDataPtr = arena.data;
    Arena_Alloc(&arena, 1024);
    ASSERT_NE(arenaDataPtr, NULL);
    ASSERT_EQ(arena.size, 1024);
    ASSERT_EQ(arena.capacity, 4096);

    Arena_Reset(&arena);
    EXPECT_EQ(arena.data, arenaDataPtr);
    EXPECT_EQ(arena.size, 0);
    EXPECT_EQ(arena.capacity, 4096);
}

TEST(ArenaTest_Free)
{
    Arena arena = Arena_New();
    ASSERT_NE(arena.data, NULL);

    Arena_Free(&arena);
    EXPECT_EQ(arena.data, NULL);
    EXPECT_EQ(arena.size, 0);
    EXPECT_EQ(arena.capacity, 0);
}
