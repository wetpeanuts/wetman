#include <wetman/utils/str.h>
#include <wetman/test/utils/macro.h>

#include <wetman/utils/mod.c>


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

TEST(StrTest_FromCStr_EmptyStr)
{
    Str str = Str_FromCStr("");
    
    EXPECT_EQ(str.len, 0);
}

TEST(StrTest_FromCStr_NonEmptyStr)
{
    Str str = Str_FromCStr("test");
    
    EXPECT_EQ(str.len, 4);
}

