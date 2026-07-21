#ifndef WETMAN_UTILS_STR_H
#define WETMAN_UTILS_STR_H

#include <stdint.h>


typedef struct {
    const char* data;
    uint32_t len;
} Str;

Str Str_FromCStr(const char* str);

int Str_EqStr(Str s1, Str s2);
int Str_EqCStr(Str s1, const char* s2);

#endif // WETMAN_UTILS_STR_H
