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

TEST(IntegrationTest_Task_List)
{
    char parent[256];
    CREATE_TMP_DIR(parent);

    char projPath[512];
    ASSERT_EQ(CreateProjectDir(parent, "projList", projPath, sizeof(projPath)), 0);

    char outInit[512];
    snprintf(outInit, sizeof(outInit), "%s/init.txt", parent);
    char outNewOne[512];
    snprintf(outNewOne, sizeof(outNewOne), "%s/new_one.txt", parent);
    char outNewTwo[512];
    snprintf(outNewTwo, sizeof(outNewTwo), "%s/new_two.txt", parent);
    char outList[512];
    snprintf(outList, sizeof(outList), "%s/list.txt", parent);
    char outListNamed[512];
    snprintf(outListNamed, sizeof(outListNamed), "%s/list_named.txt", parent);

    usize workspaceId = 0;
    EXPECT_EQ(RunWetman(projPath, "workspace init", outInit), 0);
    {
        char buf[256];
        ASSERT_EQ(ReadFile(outInit, buf, sizeof(buf)), 0);
        ASSERT_EQ(ParseInitId(buf, &workspaceId), 0);
    }

    usize taskIdOne = 0;
    EXPECT_EQ(RunWetman(projPath, "task new list_task_one", outNewOne), 0);
    {
        char buf[256];
        ASSERT_EQ(ReadFile(outNewOne, buf, sizeof(buf)), 0);
        ASSERT_EQ(ParseTaskId(buf, &taskIdOne), 0);
    }

    usize taskIdTwo = 0;
    EXPECT_EQ(RunWetman(projPath, "task new list_task_two", outNewTwo), 0);
    {
        char buf[256];
        ASSERT_EQ(ReadFile(outNewTwo, buf, sizeof(buf)), 0);
        ASSERT_EQ(ParseTaskId(buf, &taskIdTwo), 0);
    }

    EXPECT_NE(taskIdOne, taskIdTwo);

    EXPECT_EQ(RunWetman(projPath, "task list", outList), 0);
    {
        char buf[8192];
        ASSERT_EQ(ReadFile(outList, buf, sizeof(buf)), 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("ID")) >= 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("Name")) >= 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("Path")) >= 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("list_task_one")) >= 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("list_task_two")) >= 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("task.sh")) >= 0);
    }

    char listNamedCmd[256];
    snprintf(listNamedCmd, sizeof(listNamedCmd),
            "task list -w %llu",
            (unsigned long long)workspaceId);
    EXPECT_EQ(RunWetman(NULL, listNamedCmd, outListNamed), 0);
    {
        char buf[8192];
        ASSERT_EQ(ReadFile(outListNamed, buf, sizeof(buf)), 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("list_task_one")) >= 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("list_task_two")) >= 0);
    }
}

TEST(IntegrationTest_Task_List_Empty)
{
    char parent[256];
    CREATE_TMP_DIR(parent);

    char projPath[512];
    ASSERT_EQ(CreateProjectDir(parent, "projListEmpty", projPath, sizeof(projPath)), 0);

    char outInit[512];
    snprintf(outInit, sizeof(outInit), "%s/init.txt", parent);
    char outList[512];
    snprintf(outList, sizeof(outList), "%s/list.txt", parent);

    usize workspaceId = 0;
    EXPECT_EQ(RunWetman(projPath, "workspace init", outInit), 0);
    {
        char buf[256];
        ASSERT_EQ(ReadFile(outInit, buf, sizeof(buf)), 0);
        ASSERT_EQ(ParseInitId(buf, &workspaceId), 0);
    }

    char listNamedCmd[256];
    snprintf(listNamedCmd, sizeof(listNamedCmd),
            "task list -w %llu",
            (unsigned long long)workspaceId);
    EXPECT_EQ(RunWetman(NULL, listNamedCmd, outList), 0);
    {
        char buf[8192];
        ASSERT_EQ(ReadFile(outList, buf, sizeof(buf)), 0);
        Str haystack = Str_FromCStr(buf);
        ASSERT(Str_Contains(haystack, Str_FromCStr("ID")) >= 0);
        ASSERT(Str_Contains(haystack, Str_FromCStr("Name")) >= 0);
        ASSERT(Str_Contains(haystack, Str_FromCStr("Path")) >= 0);
        ASSERT(Str_Contains(haystack, Str_FromCStr("0")) < 0);
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

TEST(IntegrationTest_Task_Delete)
{
    char parent[256];
    CREATE_TMP_DIR(parent);

    char projPath[512];
    ASSERT_EQ(CreateProjectDir(parent, "projDelete", projPath, sizeof(projPath)), 0);

    char outInit[512];
    snprintf(outInit, sizeof(outInit), "%s/init.txt", parent);
    char outNew[512];
    snprintf(outNew, sizeof(outNew), "%s/new.txt", parent);
    char outDelete[512];
    snprintf(outDelete, sizeof(outDelete), "%s/delete.txt", parent);
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
    EXPECT_EQ(RunWetman(projPath, "task new delete_me", outNew), 0);
    {
        char buf[256];
        ASSERT_EQ(ReadFile(outNew, buf, sizeof(buf)), 0);
        ASSERT_EQ(ParseTaskId(buf, &taskId), 0);
    }

    EXPECT_EQ(RunWetman(projPath, "task delete 0", outDelete), 0);
    {
        char buf[512];
        ASSERT_EQ(ReadFile(outDelete, buf, sizeof(buf)), 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("deleted")) >= 0);
    }

    EXPECT_EQ(RunWetman(projPath, "task get 0", outGetMiss), 4);
    {
        char buf[8192];
        ASSERT_EQ(ReadFile(outGetMiss, buf, sizeof(buf)), 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("status code: 4")) >= 0);
    }

    usize taskIdTwo = 0;
    EXPECT_EQ(RunWetman(projPath, "task new second_task", outNew), 0);
    {
        char buf[256];
        ASSERT_EQ(ReadFile(outNew, buf, sizeof(buf)), 0);
        ASSERT_EQ(ParseTaskId(buf, &taskIdTwo), 0);
    }

    char outDeleteNamed[512];
    snprintf(outDeleteNamed, sizeof(outDeleteNamed), "%s/delete_named.txt", parent);
    char deleteNamedCmd[256];
    snprintf(deleteNamedCmd, sizeof(deleteNamedCmd),
            "task delete -w %llu %llu",
            (unsigned long long)workspaceId,
            (unsigned long long)taskIdTwo);
    EXPECT_EQ(RunWetman(NULL, deleteNamedCmd, outDeleteNamed), 0);
    {
        char buf[512];
        ASSERT_EQ(ReadFile(outDeleteNamed, buf, sizeof(buf)), 0);
        ASSERT(Str_Contains(Str_FromCStr(buf), Str_FromCStr("deleted")) >= 0);
    }
}
