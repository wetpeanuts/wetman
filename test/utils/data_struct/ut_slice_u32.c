#include <wetman/utils/data_struct/slice_u32.h>
#include <wetman/utils/test/macro.h>

#include <stddef.h>


static int SliceU32_ContentsEq(SliceU32 slice, const u32* expected, usize len)
{
    if (slice.len != len) {
        return 0;
    }
    for (usize i = 0; i < len; ++i) {
        if (slice.data[i] != expected[i]) {
            return 0;
        }
    }
    return 1;
}


TEST(SliceU32Test_CreateEmpty)
{
    SliceU32 slice = SliceU32_CreateEmpty();

    EXPECT_EQ(slice.len, 0);
    EXPECT_EQ(slice.data, (u32*)NULL);
    EXPECT_EQ(slice.__arenaPage, (void*)NULL);
}

TEST(SliceU32Test_FromData)
{
    u32 data[] = { 1, 2, 3 };
    SliceU32 slice = SliceU32_FromData(data, 3);

    EXPECT_EQ(slice.len, 3);
    EXPECT_EQ(slice.data, data);
    EXPECT_EQ(slice.__arenaPage, (void*)NULL);

    u32 expected[] = { 1, 2, 3 };
    EXPECT(SliceU32_ContentsEq(slice, expected, 3));
}

TEST(SliceU32Test_FromDataWithPage)
{
    u32 data[] = { 9, 8 };
    void* page = (void*)0x1234;
    SliceU32 slice = SliceU32_FromDataWithPage(data, 2, page);

    EXPECT_EQ(slice.len, 2);
    EXPECT_EQ(slice.data, data);
    EXPECT_EQ(slice.__arenaPage, page);

    u32 expected[] = { 9, 8 };
    EXPECT(SliceU32_ContentsEq(slice, expected, 2));
}

TEST(SliceU32Test_At)
{
    u32 data[] = { 10, 20, 30 };
    SliceU32 slice = SliceU32_FromData(data, 3);

    EXPECT_EQ(*SliceU32_At(&slice, 0), 10);
    EXPECT_EQ(*SliceU32_At(&slice, 1), 20);
    EXPECT_EQ(*SliceU32_At(&slice, 2), 30);
}

TEST(SliceU32Test_AtOutOfRange)
{
    u32 data[] = { 10, 20 };
    SliceU32 slice = SliceU32_FromData(data, 2);

    EXPECT_EQ(SliceU32_At(&slice, 2), (u32*)NULL);
    EXPECT_EQ(SliceU32_At(&slice, 100), (u32*)NULL);

    SliceU32 empty = SliceU32_CreateEmpty();
    EXPECT_EQ(SliceU32_At(&empty, 0), (u32*)NULL);
}

TEST(SliceU32Test_ConcatBothEmpty)
{
    Arena arena = Arena_New();

    SliceU32 result = SliceU32_Concat(
        SliceU32_CreateEmpty(), SliceU32_CreateEmpty(), &arena);
    EXPECT_EQ(result.len, 0);
    EXPECT_EQ(result.data, (u32*)NULL);

    Arena_Free(&arena);
}

TEST(SliceU32Test_ConcatS2Empty)
{
    Arena arena = Arena_New();

    u32 d1[] = { 1, 2, 3 };
    SliceU32 s1 = SliceU32_FromData(d1, 3);
    SliceU32 result = SliceU32_Concat(
        s1, SliceU32_CreateEmpty(), &arena);

    EXPECT_EQ(result.len, 3);
    u32 expected[] = { 1, 2, 3 };
    EXPECT(SliceU32_ContentsEq(result, expected, 3));
    // s1 is external - must copy
    EXPECT_NE(result.data, d1);
    EXPECT_NE(result.__arenaPage, (void*)NULL);

    Arena_Free(&arena);
}

TEST(SliceU32Test_ConcatS1Empty)
{
    Arena arena = Arena_New();

    u32 d2[] = { 4, 5 };
    SliceU32 s2 = SliceU32_FromData(d2, 2);
    SliceU32 result = SliceU32_Concat(
        SliceU32_CreateEmpty(), s2, &arena);

    EXPECT_EQ(result.len, 2);
    u32 expected[] = { 4, 5 };
    EXPECT(SliceU32_ContentsEq(result, expected, 2));
    // s2 is external - must copy
    EXPECT_NE(result.data, d2);
    EXPECT_NE(result.__arenaPage, (void*)NULL);

    Arena_Free(&arena);
}

TEST(SliceU32Test_ConcatS2EmptyArenaBacked)
{
    Arena arena = Arena_New();

    void* page = NULL;
    u32* data = (u32*)Arena_AllocWithPage(&arena, 2 * sizeof(u32), &page);
    data[0] = 7;
    data[1] = 8;

    // s1 arena-backed, s2 empty -> reuse s1's data directly (no copy)
    SliceU32 s1 = SliceU32_FromDataWithPage(data, 2, page);
    SliceU32 result = SliceU32_Concat(
        s1, SliceU32_CreateEmpty(), &arena);

    EXPECT_EQ(result.data, data);
    EXPECT_EQ(result.len, 2);
    EXPECT_EQ(result.__arenaPage, page);

    Arena_Free(&arena);
}

TEST(SliceU32Test_ConcatS1EmptyArenaBacked)
{
    Arena arena = Arena_New();

    void* page = NULL;
    u32* data = (u32*)Arena_AllocWithPage(&arena, 1 * sizeof(u32), &page);
    data[0] = 9;

    // s1 empty, s2 arena-backed -> reuse s2's data directly (no copy)
    SliceU32 s2 = SliceU32_FromDataWithPage(data, 1, page);
    SliceU32 result = SliceU32_Concat(
        SliceU32_CreateEmpty(), s2, &arena);

    EXPECT_EQ(result.data, data);
    EXPECT_EQ(result.len, 1);
    EXPECT_EQ(result.__arenaPage, page);

    Arena_Free(&arena);
}

TEST(SliceU32Test_ConcatSequentialSamePage)
{
    // Allocating s1 and s2 back to back on the same page, then concatenating
    // must hit the zero-copy fast path.
    Arena arena = Arena_WithPageCapacity(1024);

    void* page = NULL;
    u32* data1 = (u32*)Arena_AllocWithPage(&arena, 2 * sizeof(u32), &page);
    data1[0] = 1;
    data1[1] = 2;
    void* page2 = NULL;
    u32* data2 = (u32*)Arena_AllocWithPage(&arena, 2 * sizeof(u32), &page2);
    data2[0] = 3;
    data2[1] = 4;

    // Verify they are sequential on the same page
    ASSERT_EQ(page, page2);
    ASSERT_EQ(data1 + 2, data2);

    SliceU32 s1 = SliceU32_FromDataWithPage(data1, 2, page);
    SliceU32 s2 = SliceU32_FromDataWithPage(data2, 2, page);
    SliceU32 result = SliceU32_Concat(s1, s2, &arena);

    u32 expected[] = { 1, 2, 3, 4 };
    EXPECT(SliceU32_ContentsEq(result, expected, 4));
    // zero-copy: result aliases s1's storage
    EXPECT_EQ(result.data, data1);

    Arena_Free(&arena);
}

TEST(SliceU32Test_ConcatSamePageNotSequential)
{
    // Both slices on the same arena page but NOT sequential in memory.
    // Must NOT hit the zero-copy path.
    Arena arena = Arena_WithPageCapacity(1024);

    void* page = NULL;
    u32* data1 = (u32*)Arena_AllocWithPage(&arena, 2 * sizeof(u32), &page);
    data1[0] = 1;
    data1[1] = 2;

    // Allocate a gap so data2 does NOT immediately follow data1
    Arena_Alloc(&arena, 256);

    void* page2 = NULL;
    u32* data2 = (u32*)Arena_AllocWithPage(&arena, 2 * sizeof(u32), &page2);
    data2[0] = 3;
    data2[1] = 4;

    ASSERT_EQ(page, page2);
    ASSERT(data1 + 2 != data2);

    SliceU32 s1 = SliceU32_FromDataWithPage(data1, 2, page);
    SliceU32 s2 = SliceU32_FromDataWithPage(data2, 2, page);
    SliceU32 result = SliceU32_Concat(s1, s2, &arena);

    u32 expected[] = { 1, 2, 3, 4 };
    EXPECT(SliceU32_ContentsEq(result, expected, 4));
    EXPECT_EQ(result.len, 4);

    Arena_Free(&arena);
}

TEST(SliceU32Test_ConcatDifferentPages)
{
    // s1 and s2 on different arena pages must never use the zero-copy path.
    Arena arena = Arena_WithPageCapacity(64);

    void* page1 = NULL;
    u32* data1 = (u32*)Arena_AllocWithPage(&arena, 2 * sizeof(u32), &page1);
    data1[0] = 1;
    data1[1] = 2;

    // Force a page break by exceeding capacity
    Arena_Alloc(&arena, 64);

    void* page2 = NULL;
    u32* data2 = (u32*)Arena_AllocWithPage(&arena, 2 * sizeof(u32), &page2);
    data2[0] = 3;
    data2[1] = 4;

    ASSERT_NE(page1, page2);

    SliceU32 s1 = SliceU32_FromDataWithPage(data1, 2, page1);
    SliceU32 s2 = SliceU32_FromDataWithPage(data2, 2, page2);
    SliceU32 result = SliceU32_Concat(s1, s2, &arena);

    u32 expected[] = { 1, 2, 3, 4 };
    EXPECT(SliceU32_ContentsEq(result, expected, 4));
    // must have been copied to a new allocation
    EXPECT_NE(result.data, data1);

    Arena_Free(&arena);
}

TEST(SliceU32Test_ConcatExternalInputs)
{
    // Both inputs are external (no arena page) - must always copy.
    Arena arena = Arena_New();

    u32 d1[] = { 1, 2 };
    u32 d2[] = { 3, 4, 5 };
    SliceU32 s1 = SliceU32_FromData(d1, 2);
    SliceU32 s2 = SliceU32_FromData(d2, 3);
    SliceU32 result = SliceU32_Concat(s1, s2, &arena);

    u32 expected[] = { 1, 2, 3, 4, 5 };
    EXPECT(SliceU32_ContentsEq(result, expected, 5));
    EXPECT_NE(result.data, d1);
    EXPECT_NE(result.data, d2);
    EXPECT_NE(result.__arenaPage, (void*)NULL);

    Arena_Free(&arena);
}
