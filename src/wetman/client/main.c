#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <wetman/client/context.h>
#include <wetman/client/commands/parser.h>
#include <wetman/client/commands/healthcheck.h>
#include <wetman/client/commands/workspace_init.h>
#include <wetman/client/commands/workspace_delete.h>

#include <wetman/client/mod.c>


static void PrintUsage(FILE* out)
{
    fprintf(out,
            "Usage: wetman <command> [options]\n"
            "\n"
            "Commands:\n"
            "  healthcheck              Check server health\n"
            "  workspace init           Initialize a workspace in the current directory\n"
            "  workspace delete         Delete the current workspace\n"
            "\n"
            "Options:\n"
            "  -n, --name <name>        Workspace name (default: current directory name)\n"
            "  -w, --workspace <id>     Workspace id to delete\n"
            "  -h, --help               Show this help message and exit\n");
}

int main(int argc, char** argv)
{
    ClientContext_Init();

    CommandParser parser = CommandParser_New();
    CommandParser_RegisterCommand(&parser, Command_HealthCheck_Create(&globalClientContext.arena));
    CommandParser_RegisterCommand(&parser, Command_WorkspaceInit_Create(&globalClientContext.arena));
    CommandParser_RegisterCommand(&parser, Command_WorkspaceDelete_Create(&globalClientContext.arena));

    if (argc < 2 ||
            strcmp(argv[1], "-h") == 0 ||
            strcmp(argv[1], "--help") == 0) {
        PrintUsage(stdout);
        ClientContext_Destroy();
        return 0;
    }

    Command* cmd = CommandParser_Parse(&parser, argc, argv);
    if (cmd == NULL) {
        fprintf(stderr, "Unknown command\n");
        PrintUsage(stderr);
        ClientContext_Destroy();
        return 1;
    }

    i32 result = cmd->handler(&cmd->args, &globalClientContext.arena);

    ClientContext_Destroy();
    return (int)result;
}
