#ifndef WETMAN_TEST_INTEGRATION_UTILS_H
#define WETMAN_TEST_INTEGRATION_UTILS_H

#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/proc/subprocess.h>
#include <wetman/utils/type.h>

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


static void GetAbsolutePath(const char* relPath, char* buf, usize bufLen)
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

static i32 RunWetman(const char* dir, const char* args, const char* outFile)
{
    char wetmanPath[1024];
    GetAbsolutePath("../build/wetman", wetmanPath, sizeof(wetmanPath));

    char dirPath[1024];
    if (dir != NULL) {
        GetAbsolutePath(dir, dirPath, sizeof(dirPath));
    }

    char outPath[1024];
    GetAbsolutePath(outFile, outPath, sizeof(outPath));

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

static int ReadFile(
        const char* path,
        char*       buf,
        usize       bufLen)
{
    buf[0] = '\0';

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        return -1;
    }

    usize read = fread(buf, 1, bufLen - 1, file);
    buf[read] = '\0';

    fclose(file);
    return 0;
}

static int ParseInitId(
        const char* buf,
        usize*      id)
{
    unsigned long long parsed = 0;
    if (sscanf(buf, "Workspace initialized with id: %llu", &parsed) != 1) {
        return -1;
    }
    *id = (usize)parsed;
    return 0;
}

static int ParseTaskId(
        const char* buf,
        usize*      id)
{
    unsigned long long parsed = 0;
    if (sscanf(buf, "Task created with id: %llu", &parsed) != 1) {
        return -1;
    }
    *id = (usize)parsed;
    return 0;
}

static int CreateProjectDir(
        const char* parent,
        const char* name,
        char*       pathBuf,
        size_t      pathBufLen)
{
    snprintf(pathBuf, pathBufLen, "%s/%s", parent, name);
    if (mkdir(pathBuf, 0755) != 0) {
        return -1;
    }
    return 0;
}

static int CheckWorkspaceListContains(
        const char* parent,
        const char* outFileName,
        const char* needle)
{
    char outPath[512];
    snprintf(outPath, sizeof(outPath), "%s/%s", parent, outFileName);

    char buf[8192];
    if (ReadFile(outPath, buf, sizeof(buf)) != 0) {
        return 0;
    }

    Str haystack = Str_FromCStr(buf);
    Str n = Str_FromCStr(needle);
    return Str_Contains(haystack, n) >= 0;
}

static int CheckWorkspaceListNotContains(
        const char* parent,
        const char* outFileName,
        const char* needle)
{
    return !CheckWorkspaceListContains(parent, outFileName, needle);
}

static int CheckWorkspaceListEmpty(
        const char* parent,
        const char* outFileName,
        const char* first,
        const char* second)
{
    char outPath[512];
    snprintf(outPath, sizeof(outPath), "%s/%s", parent, outFileName);

    char buf[8192];
    if (ReadFile(outPath, buf, sizeof(buf)) != 0) {
        return 0;
    }

    Str haystack = Str_FromCStr(buf);
    int hasHeader = Str_Contains(haystack, Str_FromCStr("ID")) >= 0
                 && Str_Contains(haystack, Str_FromCStr("Name")) >= 0
                 && Str_Contains(haystack, Str_FromCStr("Path")) >= 0;
    int hasFirst  = Str_Contains(haystack, Str_FromCStr(first)) >= 0;
    int hasSecond = Str_Contains(haystack, Str_FromCStr(second)) >= 0;

    return hasHeader && !hasFirst && !hasSecond;
}

#endif // WETMAN_TEST_INTEGRATION_UTILS_H
