#ifndef WETMAN_CLIENT_COMMANDS_COMMAND_H
#define WETMAN_CLIENT_COMMANDS_COMMAND_H


#include <wetman/utils/data_struct/slice_str.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/client/args/args.h>


typedef i32(*CommandHandler)(const Args* args, Arena* arena);

typedef struct Command {
    SliceStr       prefix;
    Args           args;
    CommandHandler handler;
} Command;

#endif // WETMAN_CLIENT_COMMANDS_COMMAND_H

