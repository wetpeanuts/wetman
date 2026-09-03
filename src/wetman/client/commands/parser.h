#ifndef WETMAN_CLIENT_COMMANDS_PARSER_H
#define WETMAN_CLIENT_COMMANDS_PARSER_H


#include <wetman/client/commands/command.h>
#include <wetman/utils/type.h>


#define COMMAND_PARSER_MAX_COMMANDS 32

typedef struct {
    Command __commands[COMMAND_PARSER_MAX_COMMANDS];
    u32     __len;
} CommandParser;

CommandParser CommandParser_New(void);
void CommandParser_RegisterCommand(CommandParser* self, Command cmd);
Command* CommandParser_Parse(CommandParser* self, int argc, char** argv);

#endif // WETMAN_CLIENT_COMMANDS_PARSER_H
