#include <wetman/utils/filesystem.h>

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


i32 FS_RemoveDirectoryRecursive(const char* path)
{
    DIR *dir = opendir(path);

    if (dir == NULL) {
        if (errno == ENOENT)
            return 0;  // Doesn't exist -> already removed

        return -1;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char child[PATH_MAX];

        i32 n = snprintf(
            child,
            sizeof(child),
            "%s/%s",
            path,
            entry->d_name
        );

        if (n < 0 || (usize)n >= sizeof(child)) {
            closedir(dir);
            errno = ENAMETOOLONG;
            return -1;
        }

        struct stat st;

        if (lstat(child, &st) == -1) {
            closedir(dir);
            return -1;
        }

        if (S_ISDIR(st.st_mode)) {
            // Recursive directory
            if (FS_RemoveDirectoryRecursive(child) == -1) {
                closedir(dir);
                return -1;
            }
        } else {
            // Regular file, symlink, socket, etc.
            if (unlink(child) == -1) {
                closedir(dir);
                return -1;
            }
        }
    }

    closedir(dir);

    return rmdir(path);
}

i32 FS_OpenFile(Str path, i32 flags)
{
    if (path.len >= PATH_MAX) {
        return -1;
    }

    char buf[PATH_MAX];
    snprintf(buf, PATH_MAX - 1, "%.*s", (i32)path.len, path.data);

    return open(buf, flags, 0644);
}

i32 FS_CheckExists(Str path)
{
    char buf[PATH_MAX];
    snprintf(buf, PATH_MAX - 1, "%.*s", (i32)path.len, path.data);
    struct stat st;

    return stat(buf, &st) == 0;
}

i32 FS_CreateDir(Str path)
{
    char buf[PATH_MAX];
    snprintf(buf, PATH_MAX - 1, "%.*s", (i32)path.len, path.data);

    struct stat st;
    if (stat(buf, &st) == 0) {
        return S_ISDIR(st.st_mode) ? 0 : 0;
    }

    for (char* p = buf + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(buf, 0755) != 0 && errno != EEXIST) {
                return 0;
            }
            *p = '/';
        }
    }

    return mkdir(buf, 0755) == 0 || errno == EEXIST;
}

i32 FS_CreateSymlink(Str targetPath, Str linkPath)
{
    if (targetPath.len >= PATH_MAX || linkPath.len >= PATH_MAX) {
        return -1;
    }

    char targetBuf[PATH_MAX];
    snprintf(targetBuf, PATH_MAX, "%.*s", (i32)targetPath.len, targetPath.data);

    char linkBuf[PATH_MAX];
    snprintf(linkBuf, PATH_MAX, "%.*s", (i32)linkPath.len, linkPath.data);

    const char* symlinkTarget = targetBuf;
    char absoluteTarget[PATH_MAX];
    if (targetBuf[0] != '/' && realpath(targetBuf, absoluteTarget) == NULL) {
        return -1;
    }
    if (targetBuf[0] != '/') {
        symlinkTarget = absoluteTarget;
    }

    unlink(linkBuf);

    return symlink(symlinkTarget, linkBuf);
}

Str FS_PathJoin(Str p1, Str p2, Arena* arena)
{
    Str sep = Str_FromCStr("/");
    Str tmp = Str_Concat(p1, sep, arena);
    return Str_Concat(tmp, p2, arena);
}
