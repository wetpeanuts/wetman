#include <wetman/utils/data_struct/str.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

SLICE_IMPL(Str, char)

Str Str_FromCStr(const char* cStr)
{
    Str str = {
        .data        = (char*)cStr,
        .len         = strlen(cStr),
        .__arenaPage = NULL,
    };
    return str;
}

Str Str_FromU64(u64 value, Arena* arena)
{
    char buf[32];
    int len = snprintf(buf, sizeof(buf), "%llu", (unsigned long long)value);
    if (len <= 0) {
        return Str_FromCStr("");
    }

    void* page = NULL;
    char* data = Arena_AllocWithPage(arena, (usize)len, &page);
    memcpy(data, buf, (usize)len);

    Str str = {
        .data        = data,
        .len         = (usize)len,
        .__arenaPage = page,
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

