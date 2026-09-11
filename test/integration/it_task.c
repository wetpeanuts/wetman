#include <wetman/utils/test/macro.h>

#include "utils.h"

#include <stdio.h>
#include <string.h>


TEST(IntegrationTest_Task_Get)
{
    char parent[256];
    CREATE_TMP_DIR(parent);

    char projPath[512];
    ASSERT_EQ(CreateProjectDir(parent, "projTask", projPath, sizeof(projPath)), 0);

    char outInit[512];
    snprintf(outInit, sizeof(outInit), "%s/init.txt", parent);
    char outNew[512];
    snprintf(outNew, sizeof(outNew), "%s/new.txt", parent);
    char outGet[512];
    snprintf(outGet, sizeof(outGet), "%s/get.txt", parent);
    char outGetNamed[512];
    snprintf(outGetNamed, sizeof(outGetNamed), "%s/get_named.txt", parent);
    char outGetMiss[512];
    snprintf(outGetMiss, sizeof(outGetMiss), "%s/get_miss.txt", parent);

    usize workspaceId = 0;
    EXPECT_EQ(RunWetman(projPath, "workspace init", outInit), 0);
    {
        char buf[256];
        ASSERT_EQ(ReadFile(outInit, buf, sizeof(buf)), 0);
        ASSERT_EQ(ParseInitId(buf, &workspaceId), 0);
    }

    usize taskId = 0;
    EXPECT_EQ(RunWetman(projPath, "task new test_task", outNew), 0);
    {
        char buf[256];
        ASSERT_EQ(ReadFile(outNew, buf, sizeof(buf)), 0);
        ASSERT_EQ(ParseTaskId(buf, &taskId), 0);
    }

    EXPECT_EQ(RunWetman(projPath, "task get 0", outGet), 0);
    {
        char buf[8192];
        ASSERT_EQ(ReadFile(outGet, buf, sizeof(buf)), 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("test_task")) >= 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("task.sh")) >= 0);
    }

    char getNamedCmd[256];
    snprintf(getNamedCmd, sizeof(getNamedCmd),
            "task get -w %llu %llu",
            (unsigned long long)workspaceId,
            (unsigned long long)taskId);
    EXPECT_EQ(RunWetman(NULL, getNamedCmd, outGetNamed), 0);
    {
        char buf[8192];
        ASSERT_EQ(ReadFile(outGetNamed, buf, sizeof(buf)), 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("test_task")) >= 0);
    }

    EXPECT_EQ(RunWetman(projPath, "task get 99999", outGetMiss), 4);
    {
        char buf[8192];
        ASSERT_EQ(ReadFile(outGetMiss, buf, sizeof(buf)), 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("status code: 4")) >= 0);
    }
}
