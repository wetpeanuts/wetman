#ifndef WETMAN_UTILS_STR_H
#define WETMAN_UTILS_STR_H

#include <stdint.h>


typedef struct {
    const char* data;
    uint32_t len;
} StrSlice;

StrSlice StrSlice_FromCStr(const char* str);

#endif // WETMAN_UTILS_STR_H
