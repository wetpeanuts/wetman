#include <wetman/utils/str.h>
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

