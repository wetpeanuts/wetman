#include <wetman/utils/mem/arena.h>
#include <wetman/utils/test/macro.h>


TEST(ArenaTest_NewFree)
{
    Arena arena = Arena_New();
    Arena_Free(&arena);
}

TEST(ArenaTest_NewReset)
{
    Arena arena = Arena_New();

    // First allocation
    void* data = Arena_Alloc(&arena, 1024);
    ASSERT_NE(data, NULL);

    Arena_Reset(&arena);

    // Memory reused
    void* dataSameMem = Arena_Alloc(&arena, 2048);
    ASSERT_NE(dataSameMem, NULL);
    EXPECT_EQ(data, dataSameMem);

    Arena_Free(&arena);
}

TEST(ArenaTest_Alloc)
{
    Arena arena = Arena_New();

    // First allocation
    void* data = Arena_Alloc(&arena, 1024);
    ASSERT_NE(data, NULL);

    // Allocate on the same page
    void* dataSamePage = Arena_Alloc(&arena, 2048);
    ASSERT_NE(dataSamePage, NULL);
    EXPECT_EQ(data + 1024, dataSamePage);

    // Not enough space on the same page, allocate on a new page
    void* dataDiffPage = Arena_Alloc(&arena, 2048);
    ASSERT_NE(dataDiffPage, NULL);
    ASSERT_NE(dataSamePage + 2048, dataDiffPage);

    // Data chunk larger than defaul page size
    // Always takes a new page of requested size
    void* dataLarge = Arena_Alloc(&arena, 5120);
    ASSERT_NE(dataLarge, NULL);
    ASSERT_NE(dataDiffPage + 2048, dataLarge);

    // Next allocation on a new page
    void* dataAfterLarge = Arena_Alloc(&arena, 1024);
    ASSERT_NE(dataAfterLarge, NULL);
    ASSERT_NE(dataLarge + 5120, dataAfterLarge);

    Arena_Free(&arena);
}

TEST(ArenaTest_CanAllocOnSamePage)
{
    Arena arena = Arena_New();

    EXPECT_NE(Arena_CanAllocOnSamePage(&arena, 1024), NULL);
    EXPECT_NE(Arena_CanAllocOnSamePage(&arena, 4096), NULL);
    EXPECT_EQ(Arena_CanAllocOnSamePage(&arena, 5120), NULL);

    // First allocation
    void* data = Arena_Alloc(&arena, 1024);
    ASSERT_NE(data, NULL);

    // Allocate on the same page
    void* dataSamePage = Arena_Alloc(&arena, 2048);
    ASSERT_NE(dataSamePage, NULL);
    EXPECT_EQ(data + 1024, dataSamePage);

    // Not enough space on the same page, allocate on a new page
    void* dataDiffPage = Arena_Alloc(&arena, 2048);
    ASSERT_NE(dataDiffPage, NULL);
    ASSERT_NE(dataSamePage + 2048, dataDiffPage);

    // Data chunk larger than defaul page size
    // Always takes a new page of requested size
    void* dataLarge = Arena_Alloc(&arena, 5120);
    ASSERT_NE(dataLarge, NULL);
    ASSERT_NE(dataDiffPage + 2048, dataLarge);

    // Next allocation on a new page
    void* dataAfterLarge = Arena_Alloc(&arena, 1024);
    ASSERT_NE(dataAfterLarge, NULL);
    ASSERT_NE(dataLarge + 5120, dataAfterLarge);

    Arena_Free(&arena);
}

