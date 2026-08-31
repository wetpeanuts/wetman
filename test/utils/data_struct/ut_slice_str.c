#include <wetman/utils/data_struct/slice_str.h>
#include <wetman/utils/test/macro.h>


TEST(SliceStrTest_CreateEmpty)
{
    SliceStr slice = SliceStr_CreateEmpty();

    EXPECT_EQ(slice.len, 0);
    EXPECT_EQ(slice.data, (Str*)NULL);
    EXPECT_EQ(slice.__arenaPage, (void*)NULL);
}

TEST(SliceStrTest_FromData)
{
    Str items[] = { Str_FromCStr("hello"), Str_FromCStr("world") };
    SliceStr slice = SliceStr_FromData(items, 2);

    EXPECT_EQ(slice.len, 2);
    EXPECT_EQ(slice.data, items);
    EXPECT_EQ(slice.__arenaPage, (void*)NULL);

    EXPECT(Str_EqCStr(slice.data[0], "hello"));
    EXPECT(Str_EqCStr(slice.data[1], "world"));
}

TEST(SliceStrTest_FromDataWithPage)
{
    Str items[] = { Str_FromCStr("foo") };
    void* page = (void*)0x1234;
    SliceStr slice = SliceStr_FromDataWithPage(items, 1, page);

    EXPECT_EQ(slice.len, 1);
    EXPECT_EQ(slice.data, items);
    EXPECT_EQ(slice.__arenaPage, page);
    EXPECT(Str_EqCStr(slice.data[0], "foo"));
}

TEST(SliceStrTest_At)
{
    Str items[] = { Str_FromCStr("a"), Str_FromCStr("bb"), Str_FromCStr("ccc") };
    SliceStr slice = SliceStr_FromData(items, 3);

    EXPECT(Str_EqCStr(*SliceStr_At(&slice, 0), "a"));
    EXPECT(Str_EqCStr(*SliceStr_At(&slice, 1), "bb"));
    EXPECT(Str_EqCStr(*SliceStr_At(&slice, 2), "ccc"));
}

TEST(SliceStrTest_AtOutOfRange)
{
    Str items[] = { Str_FromCStr("x"), Str_FromCStr("y") };
    SliceStr slice = SliceStr_FromData(items, 2);

    EXPECT_EQ(SliceStr_At(&slice, 2), (Str*)NULL);
    EXPECT_EQ(SliceStr_At(&slice, 100), (Str*)NULL);

    SliceStr empty = SliceStr_CreateEmpty();
    EXPECT_EQ(SliceStr_At(&empty, 0), (Str*)NULL);
}

TEST(SliceStrTest_ConcatBothEmpty)
{
    Arena arena = Arena_New();

    SliceStr result = SliceStr_Concat(
        SliceStr_CreateEmpty(), SliceStr_CreateEmpty(), &arena);
    EXPECT_EQ(result.len, 0);

    Arena_Free(&arena);
}

TEST(SliceStrTest_ConcatS2Empty)
{
    Arena arena = Arena_New();

    Str items[] = { Str_FromCStr("hello"), Str_FromCStr("world") };
    SliceStr s1 = SliceStr_FromData(items, 2);
    SliceStr result = SliceStr_Concat(
        s1, SliceStr_CreateEmpty(), &arena);

    EXPECT_EQ(result.len, 2);
    EXPECT(Str_EqCStr(result.data[0], "hello"));
    EXPECT(Str_EqCStr(result.data[1], "world"));
    // s1 is external - must copy
    EXPECT_NE(result.data, items);

    Arena_Free(&arena);
}

TEST(SliceStrTest_ConcatS1Empty)
{
    Arena arena = Arena_New();

    Str items[] = { Str_FromCStr("foo") };
    SliceStr s2 = SliceStr_FromData(items, 1);
    SliceStr result = SliceStr_Concat(
        SliceStr_CreateEmpty(), s2, &arena);

    EXPECT_EQ(result.len, 1);
    EXPECT(Str_EqCStr(result.data[0], "foo"));
    EXPECT_NE(result.data, items);

    Arena_Free(&arena);
}

TEST(SliceStrTest_ConcatS2EmptyArenaBacked)
{
    Arena arena = Arena_New();

    void* page = NULL;
    Str* data = (Str*)Arena_AllocWithPage(&arena, 1 * sizeof(Str), &page);
    data[0] = Str_FromCStr("reuse");

    SliceStr s1 = SliceStr_FromDataWithPage(data, 1, page);
    SliceStr result = SliceStr_Concat(
        s1, SliceStr_CreateEmpty(), &arena);

    EXPECT_EQ(result.data, data);
    EXPECT_EQ(result.len, 1);
    EXPECT_EQ(result.__arenaPage, page);

    Arena_Free(&arena);
}

TEST(SliceStrTest_ConcatS1EmptyArenaBacked)
{
    Arena arena = Arena_New();

    void* page = NULL;
    Str* data = (Str*)Arena_AllocWithPage(&arena, 1 * sizeof(Str), &page);
    data[0] = Str_FromCStr("reuse");

    SliceStr s2 = SliceStr_FromDataWithPage(data, 1, page);
    SliceStr result = SliceStr_Concat(
        SliceStr_CreateEmpty(), s2, &arena);

    EXPECT_EQ(result.data, data);
    EXPECT_EQ(result.len, 1);
    EXPECT_EQ(result.__arenaPage, page);

    Arena_Free(&arena);
}

TEST(SliceStrTest_ConcatSequentialSamePage)
{
    Arena arena = Arena_WithPageCapacity(1024);

    void* page = NULL;
    Str* data1 = (Str*)Arena_AllocWithPage(&arena, 2 * sizeof(Str), &page);
    data1[0] = Str_FromCStr("hello");
    data1[1] = Str_FromCStr("world");
    void* page2 = NULL;
    Str* data2 = (Str*)Arena_AllocWithPage(&arena, 1 * sizeof(Str), &page2);
    data2[0] = Str_FromCStr("!");

    // Verify sequential on same page
    ASSERT_EQ(page, page2);
    ASSERT_EQ(data1 + 2, data2);

    SliceStr s1 = SliceStr_FromDataWithPage(data1, 2, page);
    SliceStr s2 = SliceStr_FromDataWithPage(data2, 1, page);
    SliceStr result = SliceStr_Concat(s1, s2, &arena);

    EXPECT_EQ(result.len, 3);
    EXPECT(Str_EqCStr(result.data[0], "hello"));
    EXPECT(Str_EqCStr(result.data[1], "world"));
    EXPECT(Str_EqCStr(result.data[2], "!"));
    // zero-copy: result aliases s1's storage
    EXPECT_EQ(result.data, data1);

    Arena_Free(&arena);
}

TEST(SliceStrTest_ConcatDifferentPages)
{
    Arena arena = Arena_WithPageCapacity(64);

    void* page1 = NULL;
    Str* data1 = (Str*)Arena_AllocWithPage(&arena, 1 * sizeof(Str), &page1);
    data1[0] = Str_FromCStr("hello");

    // Force a page break
    Arena_Alloc(&arena, 64);

    void* page2 = NULL;
    Str* data2 = (Str*)Arena_AllocWithPage(&arena, 1 * sizeof(Str), &page2);
    data2[0] = Str_FromCStr("world");

    ASSERT_NE(page1, page2);

    SliceStr s1 = SliceStr_FromDataWithPage(data1, 1, page1);
    SliceStr s2 = SliceStr_FromDataWithPage(data2, 1, page2);
    SliceStr result = SliceStr_Concat(s1, s2, &arena);

    EXPECT_EQ(result.len, 2);
    EXPECT(Str_EqCStr(result.data[0], "hello"));
    EXPECT(Str_EqCStr(result.data[1], "world"));
    EXPECT_NE(result.data, data1);

    Arena_Free(&arena);
}

TEST(SliceStrTest_ConcatExternalInputs)
{
    Arena arena = Arena_New();

    Str s1_items[] = { Str_FromCStr("a"), Str_FromCStr("b") };
    Str s2_items[] = { Str_FromCStr("c") };
    SliceStr s1 = SliceStr_FromData(s1_items, 2);
    SliceStr s2 = SliceStr_FromData(s2_items, 1);
    SliceStr result = SliceStr_Concat(s1, s2, &arena);

    EXPECT_EQ(result.len, 3);
    EXPECT(Str_EqCStr(result.data[0], "a"));
    EXPECT(Str_EqCStr(result.data[1], "b"));
    EXPECT(Str_EqCStr(result.data[2], "c"));
    EXPECT_NE(result.data, s1_items);
    EXPECT_NE(result.data, s2_items);
    EXPECT_NE(result.__arenaPage, (void*)NULL);

    Arena_Free(&arena);
}
