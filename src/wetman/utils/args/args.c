#include <wetman/utils/args/args.h>


void Args_Reset(Args* self)
{
    for (u32 i = 0; i < self->len; ++i) {
        self->args[i].value       = Str_CreateEmpty();
        self->args[i].initialized = FALSE;
    }
}
