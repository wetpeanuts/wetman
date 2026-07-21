#include <wetman/utils/str.h>

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
