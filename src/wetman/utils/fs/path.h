#ifndef WETMAN_UTILS_FS_PATH_H
#define WETMAN_UTILS_FS_PATH_H

#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/type.h>


#define PATH_MAX_LEN 1024;

typedef struct {
    char strPath[1024];
} Path;

Path Path_New(Str strPath);
i32 Path_Open(Path* path);

#endif // WETMAN_UTILS_FS_PATH_H
