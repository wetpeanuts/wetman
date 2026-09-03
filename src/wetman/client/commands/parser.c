#include <wetman/client/commands/parser.h>

#include <wetman/utils/data_struct/str.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


CommandParser CommandParser_New(void)
{
    CommandParser parser = { 
        .__len = 0,
    };
    return parser;
}

void CommandParser_RegisterCommand(CommandParser* self, Command cmd)
{
    if (self->__len >= COMMAND_PARSER_MAX_COMMANDS) {
        fprintf(stderr, "Command count exceeds max command length\n");
        exit(1);
    }
    self->__commands[self->__len++] = cmd;
}

Command* CommandParser_Parse(CommandParser* self, int argc, char** argv)
{
    for (u32 i = 0; i < self->__len; ++i) {
        Command* cmd = &self->__commands[i];

        if ((usize)argc - 1 < cmd->prefix.len) {
            continue;
        }

        int matched = 1;
        for (usize p = 0; p < cmd->prefix.len; ++p) {
            if (!Str_EqCStr(cmd->prefix.data[p], argv[1 + p])) {
                matched = 0;
                break;
            }
        }

        if (!matched) {
            continue;
        }

        int argIndex = 1 + (int)cmd->prefix.len;

        while (argIndex < argc) {
            const char* token = argv[argIndex];
            int found = 0;

            for (u32 a = 0; a < cmd->args.len; ++a) {
                Arg* arg = &cmd->args.args[a];

                if (Str_EqCStr(arg->shortForm, token) ||
                        Str_EqCStr(arg->fullForm, token)) {
                    if (argIndex + 1 >= argc) {
                        fprintf(stderr,
                                "Missing value for option: %s\n",
                                token);
                        return NULL;
                    }
                    arg->value = Str_FromCStr(argv[argIndex + 1]);
                    arg->initialized = TRUE;
                    argIndex += 2;
                    found = 1;
                    break;
                }
            }

            if (!found) {
                fprintf(stderr, "Unknown option: %s\n", token);
                return NULL;
            }
        }

        return cmd;
    }

    return NULL;
}
