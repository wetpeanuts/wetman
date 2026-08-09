#include <wetman/utils/filesystem.h>

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

i32 removeDirectoryRecursive(const char* path)
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
            if (removeDirectoryRecursive(child) == -1) {
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
