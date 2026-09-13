#include <wetman/utils/test/macro.h>

#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>


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

static int WriteEditorScript(const char* path, const char* content)
{
    FILE* file = fopen(path, "w");
    if (file == NULL) {
        return -1;
    }
    if (fprintf(file, "#!/bin/sh\necho \"%s\" > \"$1\"\n", content) <= 0) {
        fclose(file);
        return -1;
    }
    fclose(file);
    if (chmod(path, 0700) != 0) {
        return -1;
    }
    return 0;
}

TEST(IntegrationTest_Task_Edit)
{
    char parent[256];
    CREATE_TMP_DIR(parent);

    char projPath[512];
    ASSERT_EQ(CreateProjectDir(parent, "projEdit", projPath, sizeof(projPath)), 0);

    char outInit[512];
    snprintf(outInit, sizeof(outInit), "%s/init.txt", parent);
    char outNew[512];
    snprintf(outNew, sizeof(outNew), "%s/new.txt", parent);

    usize workspaceId = 0;
    EXPECT_EQ(RunWetman(projPath, "workspace init", outInit), 0);
    {
        char buf[256];
        ASSERT_EQ(ReadFile(outInit, buf, sizeof(buf)), 0);
        ASSERT_EQ(ParseInitId(buf, &workspaceId), 0);
    }

    usize taskId = 0;
    EXPECT_EQ(RunWetman(projPath, "task new edited_task", outNew), 0);
    {
        char buf[256];
        ASSERT_EQ(ReadFile(outNew, buf, sizeof(buf)), 0);
        ASSERT_EQ(ParseTaskId(buf, &taskId), 0);
    }

    char editorRel[512];
    snprintf(editorRel, sizeof(editorRel), "%s/editor.sh", parent);
    char editorPath[512];
    GetAbsolutePath(editorRel, editorPath, sizeof(editorPath));
    ASSERT_EQ(WriteEditorScript(editorPath, "task_edited_content"), 0);

    char wetmanPath[1024];
    GetAbsolutePath("../build/wetman", wetmanPath, sizeof(wetmanPath));

    char outEdit[512];
    snprintf(outEdit, sizeof(outEdit), "%s/edit.txt", parent);
    char editCmd[2048];
    snprintf(editCmd, sizeof(editCmd),
            "(cd %s && WETMAN_EDITOR=%s %s task edit 0) > %s 2>&1",
            projPath, editorPath, wetmanPath, outEdit);
    EXPECT_EQ(Subprocess_WaitFor(Subprocess_RunCommand(editCmd), 5000), 0);
    {
        char buf[512];
        ASSERT_EQ(ReadFile(outEdit, buf, sizeof(buf)), 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("")) >= 0);
    }

    char outContent[512];
    snprintf(outContent, sizeof(outContent), "%s/content.txt", parent);
    char checkCmd[2048];
    snprintf(checkCmd, sizeof(checkCmd),
            "(cd %s && %s task get 0 | awk '$1==\"path\"{print $2}' | xargs cat) > %s 2>&1",
            projPath, wetmanPath, outContent);
    EXPECT_EQ(Subprocess_WaitFor(Subprocess_RunCommand(checkCmd), 5000), 0);
    {
        char buf[512];
        ASSERT_EQ(ReadFile(outContent, buf, sizeof(buf)), 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("task_edited_content")) >= 0);
    }

    ASSERT_EQ(WriteEditorScript(editorPath, "task_edited_content_named"), 0);

    char outEditNamed[512];
    snprintf(outEditNamed, sizeof(outEditNamed), "%s/edit_named.txt", parent);
    char editNamedCmd[2048];
    snprintf(editNamedCmd, sizeof(editNamedCmd),
            "(WETMAN_EDITOR=%s %s task edit -w %llu %llu) > %s 2>&1",
            editorPath, wetmanPath,
            (unsigned long long)workspaceId,
            (unsigned long long)taskId,
            outEditNamed);
    EXPECT_EQ(Subprocess_WaitFor(Subprocess_RunCommand(editNamedCmd), 5000), 0);

    char outContentNamed[512];
    snprintf(outContentNamed, sizeof(outContentNamed), "%s/content_named.txt", parent);
    char checkNamedCmd[2048];
    snprintf(checkNamedCmd, sizeof(checkNamedCmd),
            "(%s task get -w %llu %llu | awk '$1==\"path\"{print $2}' | xargs cat) > %s 2>&1",
            wetmanPath,
            (unsigned long long)workspaceId,
            (unsigned long long)taskId,
            outContentNamed);
    EXPECT_EQ(Subprocess_WaitFor(Subprocess_RunCommand(checkNamedCmd), 5000), 0);
    {
        char buf[512];
        ASSERT_EQ(ReadFile(outContentNamed, buf, sizeof(buf)), 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("task_edited_content_named")) >= 0);
    }
}

TEST(IntegrationTest_Task_Print)
{
    char parent[256];
    CREATE_TMP_DIR(parent);

    char projPath[512];
    ASSERT_EQ(CreateProjectDir(parent, "projPrint", projPath, sizeof(projPath)), 0);

    char outInit[512];
    snprintf(outInit, sizeof(outInit), "%s/init.txt", parent);
    char outNew[512];
    snprintf(outNew, sizeof(outNew), "%s/new.txt", parent);

    usize workspaceId = 0;
    EXPECT_EQ(RunWetman(projPath, "workspace init", outInit), 0);
    {
        char buf[256];
        ASSERT_EQ(ReadFile(outInit, buf, sizeof(buf)), 0);
        ASSERT_EQ(ParseInitId(buf, &workspaceId), 0);
    }

    usize taskId = 0;
    EXPECT_EQ(RunWetman(projPath, "task new printed_task", outNew), 0);
    {
        char buf[256];
        ASSERT_EQ(ReadFile(outNew, buf, sizeof(buf)), 0);
        ASSERT_EQ(ParseTaskId(buf, &taskId), 0);
    }

    char editorRel[512];
    snprintf(editorRel, sizeof(editorRel), "%s/editor.sh", parent);
    char editorPath[512];
    GetAbsolutePath(editorRel, editorPath, sizeof(editorPath));
    ASSERT_EQ(WriteEditorScript(editorPath, "printed_marker"), 0);

    char wetmanPath[1024];
    GetAbsolutePath("../build/wetman", wetmanPath, sizeof(wetmanPath));

    char outEdit[512];
    snprintf(outEdit, sizeof(outEdit), "%s/edit.txt", parent);
    char editCmd[2048];
    snprintf(editCmd, sizeof(editCmd),
            "(cd %s && WETMAN_EDITOR=%s %s task edit 0) > %s 2>&1",
            projPath, editorPath, wetmanPath, outEdit);
    EXPECT_EQ(Subprocess_WaitFor(Subprocess_RunCommand(editCmd), 5000), 0);

    char outPrint[512];
    snprintf(outPrint, sizeof(outPrint), "%s/print.txt", parent);
    EXPECT_EQ(RunWetman(projPath, "task print 0", outPrint), 0);
    {
        char buf[512];
        ASSERT_EQ(ReadFile(outPrint, buf, sizeof(buf)), 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("printed_marker")) >= 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("Uninitialized task")) < 0);
    }

    char outPrintNamed[512];
    snprintf(outPrintNamed, sizeof(outPrintNamed), "%s/print_named.txt", parent);
    char printNamedCmd[256];
    snprintf(printNamedCmd, sizeof(printNamedCmd),
            "task print -w %llu %llu",
            (unsigned long long)workspaceId,
            (unsigned long long)taskId);
    EXPECT_EQ(RunWetman(NULL, printNamedCmd, outPrintNamed), 0);
    {
        char buf[512];
        ASSERT_EQ(ReadFile(outPrintNamed, buf, sizeof(buf)), 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("printed_marker")) >= 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("Uninitialized task")) < 0);
    }
}
