#include <wetman/utils/str_slice.h>

#include <string.h>


StrSlice StrSlice_FromCStr(const char* str)
{
    StrSlice strSlice = {
        .data = str,
        .len = strlen(str),
    };
    return strSlice;
}
