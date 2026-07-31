#include <wetman/utils/mem/arena.h>
#include <wetman/utils/test/macro.h>


TEST(ArenaTest_NewFree)
{
    Arena arena = Arena_New();
    ASSERT(Arena_IsValid(&arena));

    Arena_Free(&arena);
    ASSERT(!Arena_IsValid(&arena));
}

TEST(ArenaTest_NewReset)
{
    Arena arena = Arena_New();

    // First allocation
    void* data = Arena_Alloc(&arena, 1024);
    ASSERT_NE(data, NULL);

    Arena_Reset(&arena);
    ASSERT(Arena_IsValid(&arena));

    // Memory reused
    void* dataSameMem = Arena_Alloc(&arena, 2048);
    ASSERT_NE(dataSameMem, NULL);
    EXPECT_EQ(data, dataSameMem);

    Arena_Free(&arena);
}

TEST(ArenaTest_WithPageCapacity)
{
    Arena arena = Arena_WithPageCapacity(1024);

    ASSERT_NE(Arena_CanAllocOnSamePage(&arena, 1024), NULL);
    ASSERT_EQ(Arena_CanAllocOnSamePage(&arena, 1025), NULL);

    void* data = Arena_Alloc(&arena, 1024);
    ASSERT_NE(data, NULL);

    // Force allocation on the next page
    // Next page should have the same capacity
    data = Arena_Alloc(&arena, 1);
    ASSERT_NE(data, NULL);

    ASSERT_NE(Arena_CanAllocOnSamePage(&arena, 1023), NULL);
    ASSERT_EQ(Arena_CanAllocOnSamePage(&arena, 1024), NULL);

    Arena_Free(&arena);
}

TEST(ArenaTest_Alloc)
{
    Arena arena = Arena_New();

    // First allocation
    unsigned char* data = (unsigned char*)Arena_Alloc(&arena, 1024);
    ASSERT_NE(data, (unsigned char*)NULL);

    // Allocate on the same page
    unsigned char* dataSamePage = (unsigned char*)Arena_Alloc(&arena, 2048);
    ASSERT_NE(dataSamePage, (unsigned char*)NULL);
    EXPECT_EQ(data + 1024, dataSamePage);

    // Not enough space on the same page, allocate on a new page
    unsigned char* dataDiffPage = (unsigned char*)Arena_Alloc(&arena, 2048);
    ASSERT_NE(dataDiffPage, (unsigned char*)NULL);
    ASSERT_NE(dataSamePage + 2048, dataDiffPage);

    // Data chunk larger than defaul page size
    // Always takes a new page of requested size
    unsigned char* dataLarge = (unsigned char*)Arena_Alloc(&arena, 5120);
    ASSERT_NE(dataLarge, (unsigned char*)NULL);
    ASSERT_NE(dataDiffPage + 2048, dataLarge);

    // Next allocation on a new page
    unsigned char* dataAfterLarge = (unsigned char*)Arena_Alloc(&arena, 1024);
    ASSERT_NE(dataAfterLarge, (unsigned char*)NULL);
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
    unsigned char* data = (unsigned char*)Arena_Alloc(&arena, 1024);
    ASSERT_NE(data, (unsigned char*)NULL);

    // Allocate on the same page
    unsigned char* dataSamePage = (unsigned char*)Arena_Alloc(&arena, 2048);
    ASSERT_NE(dataSamePage, (unsigned char*)NULL);
    EXPECT_EQ(data + 1024, dataSamePage);

    // Not enough space on the same page, allocate on a new page
    unsigned char* dataDiffPage = (unsigned char*)Arena_Alloc(&arena, 2048);
    ASSERT_NE(dataDiffPage, (unsigned char*)NULL);
    ASSERT_NE(dataSamePage + 2048, dataDiffPage);

    // Data chunk larger than defaul page size
    // Always takes a new page of requested size
    unsigned char* dataLarge = (unsigned char*)Arena_Alloc(&arena, 5120);
    ASSERT_NE(dataLarge, (unsigned char*)NULL);
    ASSERT_NE(dataDiffPage + 2048, dataLarge);

    // Next allocation on a new page
    unsigned char* dataAfterLarge = (unsigned char*)Arena_Alloc(&arena, 1024);
    ASSERT_NE(dataAfterLarge, (unsigned char*)NULL);
    ASSERT_NE(dataLarge + 5120, dataAfterLarge);

    Arena_Free(&arena);
}

