#include <wetman/utils/test/macro.h>

#include "utils.h"

#include <stdio.h>
#include <string.h>


TEST(IntegrationTest_Workspace_NoServerRunning)
{
    const char* outPath = "tmp_dirs/workspace_list_no_server.txt";

    i32 statusCode = RunWetman(NULL, "workspace list", outPath);
    EXPECT_EQ(statusCode, 1);
}

TEST(IntegrationTest_Workspace_InitDelete_DefaultArgs)
{
    char parent[256];
    CREATE_TMP_DIR(parent);

    char projAPath[512];
    ASSERT_EQ(CreateProjectDir(parent, "projA", projAPath, sizeof(projAPath)), 0);

    char projBPath[512];
    ASSERT_EQ(CreateProjectDir(parent, "projB", projBPath, sizeof(projBPath)), 0);

    char outInitA[512];
    snprintf(outInitA, sizeof(outInitA), "%s/init_a.txt", parent);
    char outList[512];
    snprintf(outList, sizeof(outList), "%s/list.txt", parent);
    char outInitB[512];
    snprintf(outInitB, sizeof(outInitB), "%s/init_b.txt", parent);
    char outDeleteA[512];
    snprintf(outDeleteA, sizeof(outDeleteA), "%s/delete_a.txt", parent);
    char outDeleteMiss[512];
    snprintf(outDeleteMiss, sizeof(outDeleteMiss), "%s/delete_miss.txt", parent);
    char outDeleteB[512];
    snprintf(outDeleteB, sizeof(outDeleteB), "%s/delete_b.txt", parent);

    usize idA = 0, idB = 0;

    EXPECT_EQ(RunWetman(projAPath, "workspace init", outInitA), 0);
    {
        char buf[256];
        ASSERT_EQ(ReadFile(outInitA, buf, sizeof(buf)), 0);
        ASSERT_EQ(ParseInitId(buf, &idA), 0);
    }

    EXPECT_EQ(RunWetman(NULL, "workspace list", outList), 0);
    ASSERT(CheckWorkspaceListContains(parent, "list.txt", "projA"));

    EXPECT_EQ(RunWetman(projAPath, "workspace init", outInitA), 4);

    EXPECT_EQ(RunWetman(projBPath, "workspace init", outInitB), 0);
    {
        char buf[256];
        ASSERT_EQ(ReadFile(outInitB, buf, sizeof(buf)), 0);
        ASSERT_EQ(ParseInitId(buf, &idB), 0);
    }
    EXPECT_NE(idA, idB);

    EXPECT_EQ(RunWetman(NULL, "workspace list", outList), 0);
    ASSERT(CheckWorkspaceListContains(parent, "list.txt", "projA"));
    ASSERT(CheckWorkspaceListContains(parent, "list.txt", "projB"));

    EXPECT_EQ(RunWetman(projAPath, "workspace delete", outDeleteA), 0);

    EXPECT_EQ(RunWetman(NULL, "workspace list", outList), 0);
    ASSERT(CheckWorkspaceListContains(parent, "list.txt", "projB"));
    ASSERT(CheckWorkspaceListNotContains(parent, "list.txt", "projA"));

    char deleteMissCmd[256];
    snprintf(deleteMissCmd, sizeof(deleteMissCmd),
            "workspace delete -w %llu", (unsigned long long)idA);
    EXPECT_EQ(RunWetman(NULL, deleteMissCmd, outDeleteMiss), 4);

    EXPECT_EQ(RunWetman(projBPath, "workspace delete", outDeleteB), 0);

    EXPECT_EQ(RunWetman(NULL, "workspace list", outList), 0);
    ASSERT(CheckWorkspaceListEmpty(parent, "list.txt", "projA", "projB"));
}

TEST(IntegrationTest_Workspace_InitDelete_NamedArgs)
{
    char parent[256];
    CREATE_TMP_DIR(parent);

    char projCPath[512];
    ASSERT_EQ(CreateProjectDir(parent, "projC", projCPath, sizeof(projCPath)), 0);

    char projDPath[512];
    ASSERT_EQ(CreateProjectDir(parent, "projD", projDPath, sizeof(projDPath)), 0);

    char outInitC[512];
    snprintf(outInitC, sizeof(outInitC), "%s/init_c.txt", parent);
    char outList[512];
    snprintf(outList, sizeof(outList), "%s/list.txt", parent);
    char outInitD[512];
    snprintf(outInitD, sizeof(outInitD), "%s/init_d.txt", parent);
    char outDeleteC[512];
    snprintf(outDeleteC, sizeof(outDeleteC), "%s/delete_c.txt", parent);
    char outDeleteMiss[512];
    snprintf(outDeleteMiss, sizeof(outDeleteMiss), "%s/delete_miss.txt", parent);
    char outDeleteD[512];
    snprintf(outDeleteD, sizeof(outDeleteD), "%s/delete_d.txt", parent);

    usize idC = 0, idD = 0;

    EXPECT_EQ(RunWetman(projCPath, "workspace init -n first", outInitC), 0);
    {
        char buf[256];
        ASSERT_EQ(ReadFile(outInitC, buf, sizeof(buf)), 0);
        ASSERT_EQ(ParseInitId(buf, &idC), 0);
    }

    EXPECT_EQ(RunWetman(NULL, "workspace list", outList), 0);
    ASSERT(CheckWorkspaceListContains(parent, "list.txt", "first"));

    EXPECT_EQ(RunWetman(projCPath, "workspace init -n first", outInitC), 4);

    EXPECT_EQ(RunWetman(projDPath, "workspace init -n second", outInitD), 0);
    {
        char buf[256];
        ASSERT_EQ(ReadFile(outInitD, buf, sizeof(buf)), 0);
        ASSERT_EQ(ParseInitId(buf, &idD), 0);
    }
    EXPECT_NE(idC, idD);

    EXPECT_EQ(RunWetman(NULL, "workspace list", outList), 0);
    ASSERT(CheckWorkspaceListContains(parent, "list.txt", "first"));
    ASSERT(CheckWorkspaceListContains(parent, "list.txt", "second"));

    char deleteCCmd[256];
    snprintf(deleteCCmd, sizeof(deleteCCmd),
            "workspace delete -w %llu", (unsigned long long)idC);
    EXPECT_EQ(RunWetman(NULL, deleteCCmd, outDeleteC), 0);

    EXPECT_EQ(RunWetman(NULL, "workspace list", outList), 0);
    ASSERT(CheckWorkspaceListContains(parent, "list.txt", "second"));
    ASSERT(CheckWorkspaceListNotContains(parent, "list.txt", "first"));

    EXPECT_EQ(RunWetman(NULL, deleteCCmd, outDeleteMiss), 4);

    char deleteDCmd[256];
    snprintf(deleteDCmd, sizeof(deleteDCmd),
            "workspace delete -w %llu", (unsigned long long)idD);
    EXPECT_EQ(RunWetman(NULL, deleteDCmd, outDeleteD), 0);

    EXPECT_EQ(RunWetman(NULL, "workspace list", outList), 0);
    ASSERT(CheckWorkspaceListEmpty(parent, "list.txt", "first", "second"));
}
