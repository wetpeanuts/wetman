#include <wetman/utils/data_struct/str.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

Str Str_FromCStr(const char* cStr)
{
    Str str = {
        .data = cStr,
        .len = strlen(cStr),
    };
    return str;
}

int Str_EqStr(Str s1, Str s2)
{
    if (s1.len != s2.len) {
        return 0;
    }
    if (strncmp(s1.data, s2.data, s1.len)) {
        return 0;
    }
    return 1;
}

int Str_EqCStr(Str s1, const char* s2)
{
    return Str_EqStr(s1, Str_FromCStr(s2));
}

Str Str_Concat(Str s1, Str s2, Arena* arena)
{
    if (s1.len == 0 && s2.len == 0) {
        return Str_FromCStr("");
    }
    if (s2.len == 0) {
        char* data = Arena_Alloc(arena, s1.len);
        memcpy(data, s1.data, s1.len);

        Str result = {
            .data = data,
            .len  = s1.len,
        };
        return result;
    }
    if (s1.len == 0) {
        char* data = Arena_Alloc(arena, s2.len);
        memcpy(data, s2.data, s2.len);

        Str result = {
            .data = data,
            .len  = s2.len,
        };
        return result;
    }

    const usize newLen = s1.len + s2.len;
    void* dataCandidate = Arena_CanAllocOnSamePage(arena, s2.len);
    if (dataCandidate && s1.data + s1.len == dataCandidate) {
        // s1 and s2 can be allocated sequentially on the same arena page
        char* data = Arena_Alloc(arena, s2.len);
        memcpy(data, s2.data, s2.len);

        Str result = {
            .data = s1.data,
            .len = newLen,
        };
        return result;
    }

    char* data = Arena_Alloc(arena, newLen);
    memcpy(data, s1.data, s1.len);
    memcpy(data + s1.len, s2.data, s2.len);

    Str result = {
        .data = data,
        .len = newLen,
    };
    return result;
}
