#ifndef WETMAN_CLIENT_ARGS_ARG_H
#define WETMAN_CLIENT_ARGS_ARG_H

#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/type.h>


typedef struct {
    // Meta info for arg, defined statically
    Str shortForm;
    Str fullForm;
    i32 required;

    // Runtime arg info, parsed from cli command
    Str value;
    i32 initialized;
} Arg;

#endif // WETMAN_CLIENT_ARGS_ARG_H

