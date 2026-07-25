#ifndef WETMAN_UTILS_STR_H
#define WETMAN_UTILS_STR_H

#include <wetman/utils/mem/arena.h>

#include <stdint.h>


typedef struct {
    const char* data;
    uint32_t len;
} Str;

Str Str_FromCStr(const char* str);

int Str_EqStr(Str s1, Str s2);
int Str_EqCStr(Str s1, const char* s2);

Str Str_Concat(Str s1, Str s2, Arena* arena);

#endif // WETMAN_UTILS_STR_H
