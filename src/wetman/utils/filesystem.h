#ifndef WETMAN_UTILS_FILESYSTEM_H
#define WETMAN_UTILS_FILESYSTEM_H

#include <wetman/utils/type.h>
#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/mem/arena.h>


i32 FS_RemoveDirectoryRecursive(const char* path);
i32 FS_OpenFile(Str path, i32 flags);
i32 FS_CheckExists(Str path);
i32 FS_CreateDir(Str path);
i32 FS_CreateSymlink(Str targetPath, Str linkPath);
Str FS_PathJoin(Str p1, Str p2, Arena* arena);

#endif // WETMAN_UTILS_FILESYSTEM_H
