#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/test/macro.h>


TEST(StrTest_EqCStr)
{
    typedef struct {
        Str s1;
        const char* s2;
        int compareResult;
    } CompareData;
    
    CompareData compareData[] = {
        {
            .s1 = Str_FromCStr(""),
            .s2 = "",
            .compareResult = 1,
        },
        {
            .s1 = Str_FromCStr("test"),
            .s2 = "test",
            .compareResult = 1,
        },
        {
            .s1 = Str_FromCStr(""),
            .s2 = "test",
            .compareResult = 0,
        },
        {
            .s1 = Str_FromCStr("test"),
            .s2 = "",
            .compareResult = 0,
        }
    }; 

    size_t len = sizeof(compareData) / sizeof(compareData[0]);
    for (size_t i = 0; i < len; ++i) {
        EXPECT_EQ(Str_EqCStr(compareData[i].s1, compareData[i].s2), compareData[i].compareResult);
    }
}

TEST(StrTest_EqStr)
{
    typedef struct {
        Str s1;
        Str s2;
        int compareResult;
    } CompareData;
    
    CompareData compareData[] = {
        {
            .s1 = Str_FromCStr(""),
            .s2 = Str_FromCStr(""),
            .compareResult = 1,
        },
        {
            .s1 = Str_FromCStr("test"),
            .s2 = Str_FromCStr("test"),
            .compareResult = 1,
        },
        {
            .s1 = Str_FromCStr(""),
            .s2 = Str_FromCStr("test"),
            .compareResult = 0,
        },
        {
            .s1 = Str_FromCStr("test"),
            .s2 = Str_FromCStr(""),
            .compareResult = 0,
        }
    }; 

    size_t len = sizeof(compareData) / sizeof(compareData[0]);
    for (size_t i = 0; i < len; ++i) {
        EXPECT_EQ(Str_EqStr(compareData[i].s1, compareData[i].s2), compareData[i].compareResult);
    }
}

TEST(StrTest_FromCStr)
{
    typedef struct {
        const char* cStr;
        uint32_t len;
    } TestData;

    TestData testData[] = {
        {
            .cStr = "",
            .len = 0,
        },
        {
            .cStr = "test",
            .len = 4,
        },
    };
    
    size_t len = sizeof(testData) / sizeof(testData[0]);
    for (size_t i = 0; i < len; ++i) {
        Str str = Str_FromCStr(testData[i].cStr);
        EXPECT_EQ(str.len, testData[i].len);
    }
}

TEST(StrTest_Concat)
{
    typedef struct {
        const char* s1;
        const char* s2;
        const char* expected;
    } TestData;

    TestData testData[] = {
        {
            .s1 = "",
            .s2 = "",
            .expected = "",
        },
        {
            .s1 = "Hello",
            .s2 = "",
            .expected = "Hello",
        },
        {
            .s1 = "",
            .s2 = "World",
            .expected = "World",
        },
        {
            .s1 = "Hello",
            .s2 = "World",
            .expected = "HelloWorld",
        },
    };

    // TODO: allow customizing arena page and test allocation on different pages
    Arena arena = Arena_New();

    size_t len = sizeof(testData) / sizeof(testData[0]);
    for (size_t i = 0; i < len; ++i) {
        Str str1 = Str_FromCStr(testData[i].s1);
        Str str2 = Str_FromCStr(testData[i].s2);
        Str strExpected = Str_FromCStr(testData[i].expected);
        EXPECT(Str_EqStr(Str_Concat(str1, str2, &arena), strExpected));
        Arena_Reset(&arena);
    }

    Arena_Free(&arena);
}

