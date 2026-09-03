#ifndef WETMAN_CLIENT_ARGS_ARGS_H
#define WETMAN_CLIENT_ARGS_ARGS_H

#include <wetman/client/args/arg.h>
#include <wetman/utils/type.h>


#define CLIENT_MAX_ARG_COUNT 8

typedef struct {
    Arg args[CLIENT_MAX_ARG_COUNT];
    u32 len;
} Args;

#endif // WETMAN_CLIENT_ARGS_ARGS_H

