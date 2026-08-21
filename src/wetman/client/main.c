#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <wetman/client/endpoint/health_check.h>
#include <wetman/client/endpoint/workspace_init.h>
#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/net/client/unix_client.h>

#include <wetman/client/mod.c>


#define SOCKET_PATH "/tmp/wetman_server.sock"
#define CWD_BUFFER_SIZE 4096

static void PrintUsage(FILE* out)
{
    fprintf(out,
            "Usage: wetman <command> [options]\n"
            "\n"
            "Commands:\n"
            "  healthcheck              Check server health\n"
            "  workspace init           Initialize a workspace in the current directory\n"
            "\n"
            "Options:\n"
            "  -n, --name <name>  Workspace name (default: current directory name)\n"
            "  -h, --help         Show this help message and exit\n");
}

static int RunHealthCheck(void)
{
    Client client = UnixClient_Connect(SOCKET_PATH);

    Endpoint_HealthCheck_Request request = { 0 };
    Endpoint_HealthCheck_Response response = { 0 };

    ReturnCode returnCode = Endpoint_HealthCheck_Call(&client, &request, &response);

    client.disconnect(&client);

    if (returnCode != RETURN_CODE_OK) {
        fprintf(stderr,
                "Health check failed with status code: %d\n",
                (int)returnCode);
        return (int)returnCode;
    }

    printf("OK\n");

    return (int)returnCode;
}

static int RunWorkspaceInit(const char* name)
{
    char cwd[CWD_BUFFER_SIZE];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        fprintf(stderr, "Failed to get current working directory\n");
        return 1;
    }

    const char* directoryName = strrchr(cwd, '/');
    directoryName = (directoryName != NULL) ? directoryName + 1 : cwd;

    if (name == NULL) {
        if (*directoryName == '\0') {
            fprintf(stderr,
                    "Cannot derive workspace name from current directory\n");
            return 1;
        }
        name = directoryName;
    }

    Client client = UnixClient_Connect(SOCKET_PATH);

    Endpoint_WorkspaceInit_Request request = {
            .workspacePath = Str_FromCStr(cwd),
            .workspaceName = Str_FromCStr(name),
    };
    Endpoint_WorkspaceInit_Response response = { 0 };

    ReturnCode returnCode = Endpoint_WorkspaceInit_Call(&client, &request, &response);

    client.disconnect(&client);

    if (returnCode != RETURN_CODE_OK) {
        fprintf(stderr,
                "Workspace init failed with status code: %d\n",
                (int)returnCode);
        return (int)returnCode;
    }

    printf("Workspace initialized with id: %llu\n",
            (unsigned long long)response.workspaceId);

    return (int)returnCode;
}

static int RunWorkspaceInitCommand(int argc, char** argv)
{
    const char* name = NULL;

    for (int i = 3; i < argc; ++i) {
        if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--name") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Missing value for option: %s\n", argv[i]);
                PrintUsage(stderr);
                return 1;
            }
            name = argv[++i];
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            PrintUsage(stderr);
            return 1;
        }
    }

    return RunWorkspaceInit(name);
}

int main(int argc, char** argv)
{
    if (argc < 2 ||
            strcmp(argv[1], "-h") == 0 ||
            strcmp(argv[1], "--help") == 0) {
        PrintUsage(stdout);
        return 0;
    }

    if (strcmp(argv[1], "healthcheck") == 0) {
        return RunHealthCheck();
    }

    if (strcmp(argv[1], "workspace") == 0) {
        if (argc >= 3 && strcmp(argv[2], "init") == 0) {
            return RunWorkspaceInitCommand(argc, argv);
        }
        PrintUsage(stderr);
        return 1;
    }

    fprintf(stderr, "Unknown command: %s\n", argv[1]);
    PrintUsage(stderr);

    return 1;
}
