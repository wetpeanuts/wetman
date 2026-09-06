#include <wetman/utils/proc/subprocess.h>
#include <wetman/utils/test/macro.h>

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


static void __GetAbsolutePath(const char* relPath, char* buf, usize bufLen)
{
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        if (bufLen > 0) {
            buf[0] = '\0';
        }
        return;
    }
    snprintf(buf, bufLen, "%s/%s", cwd, relPath);
}

static i32 __RunWetman(const char* dir, const char* args, const char* outFile)
{
    char wetmanPath[1024];
    __GetAbsolutePath("../build/wetman", wetmanPath, sizeof(wetmanPath));

    char dirPath[1024];
    if (dir != NULL) {
        __GetAbsolutePath(dir, dirPath, sizeof(dirPath));
    }

    char outPath[1024];
    __GetAbsolutePath(outFile, outPath, sizeof(outPath));

    char cmd[2048];
    if (dir != NULL) {
        snprintf(cmd, sizeof(cmd),
                "(cd %s && %s %s) > %s 2>&1",
                dirPath, wetmanPath, args, outPath);
    } else {
        snprintf(cmd, sizeof(cmd),
                "%s %s > %s 2>&1", wetmanPath, args, outPath);
    }

    pid_t pid = Subprocess_RunCommand(cmd);
    if (pid == -1) {
        return -1;
    }

    return Subprocess_WaitFor(pid, 5000);
}

static void __ReadFile(
        __TestCaseContext* ctx,
        const char*        path,
        char*              buf,
        usize              bufLen)
{
    buf[0] = '\0';

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("[FAIL] %s:%d\n", __FILE__, __LINE__);
        printf("  Failed to open file: %s\n", path);
        ctx->failureCount++;
        return;
    }

    usize read = fread(buf, 1, bufLen - 1, file);
    buf[read] = '\0';

    fclose(file);
}

static void __expectContains(
        __TestCaseContext* ctx,
        const char*        buf,
        const char*        needle)
{
    if (strstr(buf, needle) == NULL) {
        printf("[FAIL] %s:%d\n", __FILE__, __LINE__);
        printf("  Expected output to contain: %s\n", needle);
        printf("  Actual output:\n%s\n", buf);
        ctx->failureCount++;
    }
}

static void __expectNotContains(
        __TestCaseContext* ctx,
        const char*        buf,
        const char*        needle)
{
    if (strstr(buf, needle) != NULL) {
        printf("[FAIL] %s:%d\n", __FILE__, __LINE__);
        printf("  Expected output NOT to contain: %s\n", needle);
        printf("  Actual output:\n%s\n", buf);
        ctx->failureCount++;
    }
}

static void __ParseInitId(
        __TestCaseContext* ctx,
        const char*        buf,
        usize*             id)
{
    unsigned long long parsed = 0;
    if (sscanf(buf, "Workspace initialized with id: %llu", &parsed) != 1) {
        printf("[FAIL] %s:%d\n", __FILE__, __LINE__);
        printf("  Failed to parse workspace id from output:\n%s\n", buf);
        ctx->failureCount++;
        return;
    }
    *id = (usize)parsed;
}

static void __CreateProjectDirs(
        __TestCaseContext* ctx,
        const char*        parent,
        char*              projAPath,
        size_t             projAPathBufLen,
        const char*        projAName,
        char*              projBPath,
        size_t             projBPathBufLen,
        const char*        projBName)
{
    snprintf(projAPath, projAPathBufLen, "%s/%s", parent, projAName);
    snprintf(projBPath, projBPathBufLen, "%s/%s", parent, projBName);

    if (mkdir(projAPath, 0755) != 0) {
        printf("[FAIL] %s:%d\n", __FILE__, __LINE__);
        printf("  Failed to create project dir: %s\n", projAPath);
        ctx->failureCount++;
    }
    if (mkdir(projBPath, 0755) != 0) {
        printf("[FAIL] %s:%d\n", __FILE__, __LINE__);
        printf("  Failed to create project dir: %s\n", projBPath);
        ctx->failureCount++;
    }
}

static void __AssertWorkspaceListContains(
        __TestCaseContext* ctx,
        const char*        parent,
        const char*        outFileName,
        const char*        needle)
{
    char outPath[512];
    snprintf(outPath, sizeof(outPath), "%s/%s", parent, outFileName);

    char buf[8192];
    __ReadFile(ctx, outPath, buf, sizeof(buf));
    __expectContains(ctx, buf, needle);
}

static void __AssertWorkspaceListNotContains(
        __TestCaseContext* ctx,
        const char*        parent,
        const char*        outFileName,
        const char*        needle)
{
    char outPath[512];
    snprintf(outPath, sizeof(outPath), "%s/%s", parent, outFileName);

    char buf[8192];
    __ReadFile(ctx, outPath, buf, sizeof(buf));
    __expectNotContains(ctx, buf, needle);
}

static void __AssertWorkspaceListEmpty(
        __TestCaseContext* ctx,
        const char*        parent,
        const char*        outFileName,
        const char*        first,
        const char*        second)
{
    char outPath[512];
    snprintf(outPath, sizeof(outPath), "%s/%s", parent, outFileName);

    char buf[8192];
    __ReadFile(ctx, outPath, buf, sizeof(buf));

    __expectContains(ctx, buf, "ID");
    __expectContains(ctx, buf, "Name");
    __expectContains(ctx, buf, "Path");
    __expectNotContains(ctx, buf, first);
    __expectNotContains(ctx, buf, second);
}


TEST(IntegrationTest_Workspace_NoServerRunning)
{
    const char* outPath = "tmp_dirs/workspace_list_no_server.txt";

    i32 statusCode = __RunWetman(NULL, "workspace list", outPath);
    EXPECT_EQ(statusCode, 1);
}

TEST(IntegrationTest_Workspace_InitDelete_DefaultArgs)
{
    char parent[256];
    CREATE_TMP_DIR(parent);

    char projAPath[512];
    char projBPath[512];
    __CreateProjectDirs(__testCaseContext, parent,
            projAPath, sizeof(projAPath), "projA",
            projBPath, sizeof(projBPath), "projB");

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

    EXPECT_EQ(__RunWetman(projAPath, "workspace init", outInitA), 0);
    {
        char buf[256];
        __ReadFile(__testCaseContext, outInitA, buf, sizeof(buf));
        __ParseInitId(__testCaseContext, buf, &idA);
    }

    EXPECT_EQ(__RunWetman(NULL, "workspace list", outList), 0);
    __AssertWorkspaceListContains(__testCaseContext, parent, "list.txt", "projA");

    EXPECT_EQ(__RunWetman(projAPath, "workspace init", outInitA), 4);

    EXPECT_EQ(__RunWetman(projBPath, "workspace init", outInitB), 0);
    {
        char buf[256];
        __ReadFile(__testCaseContext, outInitB, buf, sizeof(buf));
        __ParseInitId(__testCaseContext, buf, &idB);
    }
    EXPECT_NE(idA, idB);

    EXPECT_EQ(__RunWetman(NULL, "workspace list", outList), 0);
    __AssertWorkspaceListContains(__testCaseContext, parent, "list.txt", "projA");
    __AssertWorkspaceListContains(__testCaseContext, parent, "list.txt", "projB");

    EXPECT_EQ(__RunWetman(projAPath, "workspace delete", outDeleteA), 0);

    EXPECT_EQ(__RunWetman(NULL, "workspace list", outList), 0);
    __AssertWorkspaceListContains(__testCaseContext, parent, "list.txt", "projB");
    __AssertWorkspaceListNotContains(__testCaseContext, parent, "list.txt", "projA");

    char deleteMissCmd[256];
    snprintf(deleteMissCmd, sizeof(deleteMissCmd),
            "workspace delete -w %llu", (unsigned long long)idA);
    EXPECT_EQ(__RunWetman(NULL, deleteMissCmd, outDeleteMiss), 4);

    EXPECT_EQ(__RunWetman(projBPath, "workspace delete", outDeleteB), 0);

    EXPECT_EQ(__RunWetman(NULL, "workspace list", outList), 0);
    __AssertWorkspaceListEmpty(__testCaseContext, parent, "list.txt", "projA", "projB");
}

TEST(IntegrationTest_Workspace_InitDelete_NamedArgs)
{
    char parent[256];
    CREATE_TMP_DIR(parent);

    char projCPath[512];
    char projDPath[512];
    __CreateProjectDirs(__testCaseContext, parent,
            projCPath, sizeof(projCPath), "projC",
            projDPath, sizeof(projDPath), "projD");

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

    EXPECT_EQ(__RunWetman(projCPath, "workspace init -n first", outInitC), 0);
    {
        char buf[256];
        __ReadFile(__testCaseContext, outInitC, buf, sizeof(buf));
        __ParseInitId(__testCaseContext, buf, &idC);
    }

    EXPECT_EQ(__RunWetman(NULL, "workspace list", outList), 0);
    __AssertWorkspaceListContains(__testCaseContext, parent, "list.txt", "first");

    EXPECT_EQ(__RunWetman(projCPath, "workspace init -n first", outInitC), 4);

    EXPECT_EQ(__RunWetman(projDPath, "workspace init -n second", outInitD), 0);
    {
        char buf[256];
        __ReadFile(__testCaseContext, outInitD, buf, sizeof(buf));
        __ParseInitId(__testCaseContext, buf, &idD);
    }
    EXPECT_NE(idC, idD);

    EXPECT_EQ(__RunWetman(NULL, "workspace list", outList), 0);
    __AssertWorkspaceListContains(__testCaseContext, parent, "list.txt", "first");
    __AssertWorkspaceListContains(__testCaseContext, parent, "list.txt", "second");

    char deleteCCmd[256];
    snprintf(deleteCCmd, sizeof(deleteCCmd),
            "workspace delete -w %llu", (unsigned long long)idC);
    EXPECT_EQ(__RunWetman(NULL, deleteCCmd, outDeleteC), 0);

    EXPECT_EQ(__RunWetman(NULL, "workspace list", outList), 0);
    __AssertWorkspaceListContains(__testCaseContext, parent, "list.txt", "second");
    __AssertWorkspaceListNotContains(__testCaseContext, parent, "list.txt", "first");

    EXPECT_EQ(__RunWetman(NULL, deleteCCmd, outDeleteMiss), 4);

    char deleteDCmd[256];
    snprintf(deleteDCmd, sizeof(deleteDCmd),
            "workspace delete -w %llu", (unsigned long long)idD);
    EXPECT_EQ(__RunWetman(NULL, deleteDCmd, outDeleteD), 0);

    EXPECT_EQ(__RunWetman(NULL, "workspace list", outList), 0);
    __AssertWorkspaceListEmpty(__testCaseContext, parent, "list.txt", "first", "second");
}
