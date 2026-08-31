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

TEST(ArenaTest_AllocWithPage)
{
    Arena arena = Arena_New();

    void* page1 = NULL;
    void* page2 = NULL;
    void* page3 = NULL;

    // First allocation on the first page
    unsigned char* data1 = (unsigned char*)Arena_AllocWithPage(&arena, 1024, &page1);
    ASSERT_NE(data1, (unsigned char*)NULL);
    ASSERT_NE(page1, NULL);

    // Second allocation on the same page -> same page pointer
    unsigned char* data2 = (unsigned char*)Arena_AllocWithPage(&arena, 2048, &page2);
    ASSERT_NE(data2, (unsigned char*)NULL);
    EXPECT_EQ(page1, page2);
    EXPECT_EQ(data1 + 1024, data2);

    // Allocation too large for the page -> new page, different pointer
    unsigned char* data3 = (unsigned char*)Arena_AllocWithPage(&arena, 2048, &page3);
    ASSERT_NE(data3, (unsigned char*)NULL);
    ASSERT_NE(page1, page3);
    ASSERT_NE(data2 + 2048, data3);

    Arena_Free(&arena);
}

TEST(ArenaTest_AllocWithPageLarge)
{
    // Allocation larger than the page capacity always lands on its own fresh page
    Arena arena = Arena_WithPageCapacity(1024);

    void* page1 = NULL;
    unsigned char* data = (unsigned char*)Arena_AllocWithPage(&arena, 4096, &page1);
    ASSERT_NE(data, (unsigned char*)NULL);
    ASSERT_NE(page1, NULL);

    // Next allocation forces a new page with default (head) capacity
    void* page2 = NULL;
    unsigned char* dataNext = (unsigned char*)Arena_AllocWithPage(&arena, 512, &page2);
    ASSERT_NE(dataNext, (unsigned char*)NULL);
    ASSERT_NE(page1, page2);

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

