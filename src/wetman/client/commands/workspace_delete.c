#include <wetman/client/commands/workspace_delete.h>

#include <wetman/client/context.h>
#include <wetman/client/endpoint/workspace_delete.h>
#include <wetman/shared/persistence/workspace_config.h>
#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/filesystem.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define CWD_BUFFER_SIZE 4096


static i32 __Command_WorkspaceDelete_Impl(size_t workspaceId)
{
    Client client = globalClientContext.client;

    Endpoint_WorkspaceDelete_Request request = {
            .workspaceId = workspaceId,
    };
    Endpoint_WorkspaceDelete_Response response = { 0 };

    ReturnCode returnCode = Endpoint_WorkspaceDelete_Call(&client, &request, &response);

    if (returnCode != RETURN_CODE_OK) {
        fprintf(stderr,
                "Workspace delete failed with status code: %d\n",
                (i32)returnCode);
        return (i32)returnCode;
    }

    printf("Workspace %llu deleted\n",
            (unsigned long long)response.workspaceId);

    return 0;
}

static i32 __Command_WorkspaceDelete_Handler(const Args* args, Arena* arena)
{
    assert(args->len == 1);

    if (args->args[0].initialized) {
        size_t workspaceId = (size_t)strtoull(args->args[0].value.data, NULL, 10);
        return __Command_WorkspaceDelete_Impl(workspaceId);
    }

    char* cwdBuf = (char*)Arena_Alloc(arena, CWD_BUFFER_SIZE);
    if (getcwd(cwdBuf, CWD_BUFFER_SIZE) == NULL) {
        fprintf(stderr, "Failed to get current working directory\n");
        return 1;
    }

    Str wetmanDir = FS_PathJoin(Str_FromCStr(cwdBuf), Str_FromCStr(".wetman"), arena);
    Str configPath = FS_PathJoin(wetmanDir, Str_FromCStr("workspace.wmwscfg"), arena);

    if (!FS_CheckExists(configPath)) {
        fprintf(stderr, "No workspace found in current directory\n");
        return 1;
    }

    PersistenceStatus status;
    WorkspaceConfig config = WorkspaceConfig_Read(configPath, arena, &status);
    if (status != PERSISTENCE_STATUS_OK) {
        fprintf(stderr, "Failed to read workspace config\n");
        return 1;
    }

    return __Command_WorkspaceDelete_Impl(config.workspaceId);
}

Command Command_WorkspaceDelete_Create(Arena* arena)
{
    Args args = {
        .len = 1,
    };

    args.args[0] = (Arg) {
        .shortForm    = Str_FromCStr("-w"),
        .fullForm     = Str_FromCStr("--workspace"),
        .required     = FALSE,
        .value        = Str_CreateEmpty(),
        .initialized  = FALSE,
    };

    Str* prefixData = (Str*)Arena_Alloc(arena, sizeof(Str) * 2);
    prefixData[0] = Str_FromCStr("workspace");
    prefixData[1] = Str_FromCStr("delete");
    SliceStr prefix = SliceStr_FromData(prefixData, 2);

    Command cmd = {
        .prefix  = prefix,
        .args    = args,
        .handler = __Command_WorkspaceDelete_Handler,
    };

    return cmd;
}
