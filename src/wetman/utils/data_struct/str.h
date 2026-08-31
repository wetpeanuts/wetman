#ifndef WETMAN_UTILS_STR_H
#define WETMAN_UTILS_STR_H

#include <wetman/utils/data_struct/slice.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/type.h>


SLICE_DECLARE(Str, char)


Str Str_FromCStr(const char* str);
Str Str_FromU64(u64 value, Arena* arena);

int Str_EqStr(Str s1, Str s2);
int Str_EqCStr(Str s1, const char* s2);

#endif // WETMAN_UTILS_STR_H
