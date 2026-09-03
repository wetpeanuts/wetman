#include <wetman/client/commands/workspace_init.h>

#include <wetman/client/context.h>
#include <wetman/client/endpoint/workspace_init.h>
#include <wetman/shared/persistence/workspace_config.h>
#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/filesystem.h>
#include <wetman/utils/net/client/unix_client.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define CWD_BUFFER_SIZE 4096


i32 __Command_WorkspaceInit_Impl(
        Str workspacePath,
        Str workspaceName)
{
    Client client = globalClientContext.client;

    Endpoint_WorkspaceInit_Request request = {
            .workspacePath = workspacePath,
            .workspaceName = workspaceName,
    };
    Endpoint_WorkspaceInit_Response response = { 0 };

    ReturnCode returnCode = Endpoint_WorkspaceInit_Call(&client, &request, &response);

    if (returnCode != RETURN_CODE_OK) {
        fprintf(stderr,
                "Workspace init failed with status code: %d\n",
                (i32)returnCode);
        return (i32)returnCode;
    }

    printf("Workspace initialized with id: %llu\n",
            (unsigned long long)response.workspaceId);

    return (i32)returnCode;
}

i32 __Command_WorkspaceInit_Handler(const Args* args, Arena* arena)
{
    assert(args->len == 1);

    char* cwdBuf = (char*)Arena_Alloc(arena, CWD_BUFFER_SIZE);
    if (getcwd(cwdBuf, CWD_BUFFER_SIZE) == NULL) {
        fprintf(stderr, "Failed to get current working directory\n");
        return 1;
    }

    const char* dirName = strrchr(cwdBuf, '/');
    dirName = (dirName != NULL) ? dirName + 1 : cwdBuf;

    if (*dirName == '\0') {
        fprintf(stderr,
                "Cannot derive workspace name from current directory\n");
        return 1;
    }
    Str workspaceName = Str_FromCStr(dirName);
    Str workspacePath = Str_FromCStr(cwdBuf);

    if (args->args[0].initialized) {
        workspaceName = args->args[0].value;
    }

    return __Command_WorkspaceInit_Impl(workspacePath, workspaceName);
}

Command Command_WorkspaceInit_Create(Arena* arena)
{
    Args args = {
        .len = 1,
    };

    args.args[0] = (Arg) {
        .shortForm    = Str_FromCStr("-n"),
        .fullForm     = Str_FromCStr("--name"),
        .required     = FALSE,
        .value        = Str_CreateEmpty(),
        .initialized  = FALSE,
    };

    Str* prefixData = (Str*)Arena_Alloc(arena, sizeof(Str) * 2);
    prefixData[0] = Str_FromCStr("workspace");
    prefixData[1] = Str_FromCStr("init");
    SliceStr prefix = SliceStr_FromData(prefixData, 2);

    Command cmd = {
        .prefix  = prefix,
        .args    = args,
        .handler = __Command_WorkspaceInit_Handler,
    };

    return cmd;
}
