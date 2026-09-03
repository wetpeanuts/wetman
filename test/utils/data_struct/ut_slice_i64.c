#include <wetman/utils/data_struct/slice_i64.h>
#include <wetman/utils/test/macro.h>

#include <stddef.h>


static int SliceI64_ContentsEq(SliceI64 slice, const i64* expected, usize len)
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


TEST(SliceI64Test_CreateEmpty)
{
    SliceI64 slice = SliceI64_CreateEmpty();

    EXPECT_EQ(slice.len, 0);
    EXPECT_EQ(slice.data, (i64*)NULL);
    EXPECT_EQ(slice.__arenaPage, (void*)NULL);
}

TEST(SliceI64Test_FromData)
{
    i64 data[] = { 1, 2, 3 };
    SliceI64 slice = SliceI64_FromData(data, 3);

    EXPECT_EQ(slice.len, 3);
    EXPECT_EQ(slice.data, data);
    EXPECT_EQ(slice.__arenaPage, (void*)NULL);

    i64 expected[] = { 1, 2, 3 };
    EXPECT(SliceI64_ContentsEq(slice, expected, 3));
}

TEST(SliceI64Test_FromDataWithPage)
{
    i64 data[] = { 9, 8 };
    void* page = (void*)0x1234;
    SliceI64 slice = SliceI64_FromDataWithPage(data, 2, page);

    EXPECT_EQ(slice.len, 2);
    EXPECT_EQ(slice.data, data);
    EXPECT_EQ(slice.__arenaPage, page);

    i64 expected[] = { 9, 8 };
    EXPECT(SliceI64_ContentsEq(slice, expected, 2));
}

TEST(SliceI64Test_At)
{
    i64 data[] = { 10, -20, 30 };
    SliceI64 slice = SliceI64_FromData(data, 3);

    EXPECT_EQ(*SliceI64_At(&slice, 0), 10);
    EXPECT_EQ(*SliceI64_At(&slice, 1), -20);
    EXPECT_EQ(*SliceI64_At(&slice, 2), 30);
}

TEST(SliceI64Test_AtOutOfRange)
{
    i64 data[] = { 10, 20 };
    SliceI64 slice = SliceI64_FromData(data, 2);

    EXPECT_EQ(SliceI64_At(&slice, 2), (i64*)NULL);
    EXPECT_EQ(SliceI64_At(&slice, 100), (i64*)NULL);

    SliceI64 empty = SliceI64_CreateEmpty();
    EXPECT_EQ(SliceI64_At(&empty, 0), (i64*)NULL);
}

TEST(SliceI64Test_ConcatBothEmpty)
{
    Arena arena = Arena_New();

    SliceI64 result = SliceI64_Concat(
        SliceI64_CreateEmpty(), SliceI64_CreateEmpty(), &arena);
    EXPECT_EQ(result.len, 0);
    EXPECT_EQ(result.data, (i64*)NULL);

    Arena_Free(&arena);
}

TEST(SliceI64Test_ConcatS2Empty)
{
    Arena arena = Arena_New();

    i64 d1[] = { 1, 2, 3 };
    SliceI64 s1 = SliceI64_FromData(d1, 3);
    SliceI64 result = SliceI64_Concat(
        s1, SliceI64_CreateEmpty(), &arena);

    EXPECT_EQ(result.len, 3);
    i64 expected[] = { 1, 2, 3 };
    EXPECT(SliceI64_ContentsEq(result, expected, 3));
    // s1 is external - must copy
    EXPECT_NE(result.data, d1);
    EXPECT_NE(result.__arenaPage, (void*)NULL);

    Arena_Free(&arena);
}

TEST(SliceI64Test_ConcatS1Empty)
{
    Arena arena = Arena_New();

    i64 d2[] = { 4, 5 };
    SliceI64 s2 = SliceI64_FromData(d2, 2);
    SliceI64 result = SliceI64_Concat(
        SliceI64_CreateEmpty(), s2, &arena);

    EXPECT_EQ(result.len, 2);
    i64 expected[] = { 4, 5 };
    EXPECT(SliceI64_ContentsEq(result, expected, 2));
    // s2 is external - must copy
    EXPECT_NE(result.data, d2);
    EXPECT_NE(result.__arenaPage, (void*)NULL);

    Arena_Free(&arena);
}

TEST(SliceI64Test_ConcatS2EmptyArenaBacked)
{
    Arena arena = Arena_New();

    void* page = NULL;
    i64* data = (i64*)Arena_AllocWithPage(&arena, 2 * sizeof(i64), &page);
    data[0] = 7;
    data[1] = 8;

    // s1 arena-backed, s2 empty -> reuse s1's data directly (no copy)
    SliceI64 s1 = SliceI64_FromDataWithPage(data, 2, page);
    SliceI64 result = SliceI64_Concat(
        s1, SliceI64_CreateEmpty(), &arena);

    EXPECT_EQ(result.data, data);
    EXPECT_EQ(result.len, 2);
    EXPECT_EQ(result.__arenaPage, page);

    Arena_Free(&arena);
}

TEST(SliceI64Test_ConcatS1EmptyArenaBacked)
{
    Arena arena = Arena_New();

    void* page = NULL;
    i64* data = (i64*)Arena_AllocWithPage(&arena, 1 * sizeof(i64), &page);
    data[0] = 9;

    // s1 empty, s2 arena-backed -> reuse s2's data directly (no copy)
    SliceI64 s2 = SliceI64_FromDataWithPage(data, 1, page);
    SliceI64 result = SliceI64_Concat(
        SliceI64_CreateEmpty(), s2, &arena);

    EXPECT_EQ(result.data, data);
    EXPECT_EQ(result.len, 1);
    EXPECT_EQ(result.__arenaPage, page);

    Arena_Free(&arena);
}

TEST(SliceI64Test_ConcatSequentialSamePage)
{
    // Allocating s1 and s2 back to back on the same page, then concatenating
    // must hit the zero-copy fast path.
    Arena arena = Arena_WithPageCapacity(1024);

    void* page = NULL;
    i64* data1 = (i64*)Arena_AllocWithPage(&arena, 2 * sizeof(i64), &page);
    data1[0] = 1;
    data1[1] = 2;
    void* page2 = NULL;
    i64* data2 = (i64*)Arena_AllocWithPage(&arena, 2 * sizeof(i64), &page2);
    data2[0] = 3;
    data2[1] = 4;

    // Verify they are sequential on the same page
    ASSERT_EQ(page, page2);
    ASSERT_EQ(data1 + 2, data2);

    SliceI64 s1 = SliceI64_FromDataWithPage(data1, 2, page);
    SliceI64 s2 = SliceI64_FromDataWithPage(data2, 2, page);
    SliceI64 result = SliceI64_Concat(s1, s2, &arena);

    i64 expected[] = { 1, 2, 3, 4 };
    EXPECT(SliceI64_ContentsEq(result, expected, 4));
    // zero-copy: result aliases s1's storage
    EXPECT_EQ(result.data, data1);

    Arena_Free(&arena);
}

TEST(SliceI64Test_ConcatSamePageNotSequential)
{
    // Both slices on the same arena page but NOT sequential in memory.
    // Must NOT hit the zero-copy path.
    Arena arena = Arena_WithPageCapacity(1024);

    void* page = NULL;
    i64* data1 = (i64*)Arena_AllocWithPage(&arena, 2 * sizeof(i64), &page);
    data1[0] = 1;
    data1[1] = 2;

    // Allocate a gap so data2 does NOT immediately follow data1
    Arena_Alloc(&arena, 256);

    void* page2 = NULL;
    i64* data2 = (i64*)Arena_AllocWithPage(&arena, 2 * sizeof(i64), &page2);
    data2[0] = 3;
    data2[1] = 4;

    ASSERT_EQ(page, page2);
    ASSERT(data1 + 2 != data2);

    SliceI64 s1 = SliceI64_FromDataWithPage(data1, 2, page);
    SliceI64 s2 = SliceI64_FromDataWithPage(data2, 2, page);
    SliceI64 result = SliceI64_Concat(s1, s2, &arena);

    i64 expected[] = { 1, 2, 3, 4 };
    EXPECT(SliceI64_ContentsEq(result, expected, 4));
    EXPECT_EQ(result.len, 4);

    Arena_Free(&arena);
}

TEST(SliceI64Test_ConcatDifferentPages)
{
    // s1 and s2 on different arena pages must never use the zero-copy path.
    Arena arena = Arena_WithPageCapacity(64);

    void* page1 = NULL;
    i64* data1 = (i64*)Arena_AllocWithPage(&arena, 2 * sizeof(i64), &page1);
    data1[0] = 1;
    data1[1] = 2;

    // Force a page break by exceeding capacity
    Arena_Alloc(&arena, 64);

    void* page2 = NULL;
    i64* data2 = (i64*)Arena_AllocWithPage(&arena, 2 * sizeof(i64), &page2);
    data2[0] = 3;
    data2[1] = 4;

    ASSERT_NE(page1, page2);

    SliceI64 s1 = SliceI64_FromDataWithPage(data1, 2, page1);
    SliceI64 s2 = SliceI64_FromDataWithPage(data2, 2, page2);
    SliceI64 result = SliceI64_Concat(s1, s2, &arena);

    i64 expected[] = { 1, 2, 3, 4 };
    EXPECT(SliceI64_ContentsEq(result, expected, 4));
    // must have been copied to a new allocation
    EXPECT_NE(result.data, data1);

    Arena_Free(&arena);
}

TEST(SliceI64Test_ConcatExternalInputs)
{
    // Both inputs are external (no arena page) - must always copy.
    Arena arena = Arena_New();

    i64 d1[] = { 1, 2 };
    i64 d2[] = { 3, 4, 5 };
    SliceI64 s1 = SliceI64_FromData(d1, 2);
    SliceI64 s2 = SliceI64_FromData(d2, 3);
    SliceI64 result = SliceI64_Concat(s1, s2, &arena);

    i64 expected[] = { 1, 2, 3, 4, 5 };
    EXPECT(SliceI64_ContentsEq(result, expected, 5));
    EXPECT_NE(result.data, d1);
    EXPECT_NE(result.data, d2);
    EXPECT_NE(result.__arenaPage, (void*)NULL);

    Arena_Free(&arena);
}
