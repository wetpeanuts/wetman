#include <wetman/client/commands/parser.h>

#include <wetman/utils/args/parser.h>
#include <wetman/utils/data_struct/str.h>

#include <stdio.h>
#include <stdlib.h>


static int __CommandParser_MatchPrefix(const Command* cmd, int argc, char** argv)
{
    if ((usize)argc - 1 < cmd->prefix.len) {
        return 0;
    }

    for (usize p = 0; p < cmd->prefix.len; ++p) {
        if (!Str_EqCStr(cmd->prefix.data[p], argv[1 + p])) {
            return 0;
        }
    }

    return 1;
}

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

        if (!__CommandParser_MatchPrefix(cmd, argc, argv)) {
            continue;
        }

        ArgsParser argsParser;
        ArgsParser_Init(&argsParser, &cmd->args);
        ArgsParser_Parse(&argsParser, argc, argv, 1 + (i32)cmd->prefix.len);

        if (argsParser.status != ARGS_PARSE_STATUS_OK) {
            ArgsParser_PrintError(stderr, &argsParser);
            return NULL;
        }

        return cmd;
    }

    return NULL;
}
