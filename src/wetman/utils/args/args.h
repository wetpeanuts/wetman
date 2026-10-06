#ifndef WETMAN_UTILS_ARGS_ARGS_H
#define WETMAN_UTILS_ARGS_ARGS_H

#include <wetman/utils/args/arg.h>
#include <wetman/utils/type.h>


#define ARGS_MAX_ARG_COUNT 8

typedef struct {
    Arg args[ARGS_MAX_ARG_COUNT];
    u32 len;
} Args;

// Clears runtime arg info, keeping meta info untouched
void Args_Reset(Args* self);

#endif // WETMAN_UTILS_ARGS_ARGS_H
