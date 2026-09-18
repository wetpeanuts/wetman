#include <wetman/client/commands/task_list.h>

#include <wetman/client/context.h>
#include <wetman/client/endpoint/task_list.h>
#include <wetman/shared/persistence/workspace_config.h>
#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/filesystem.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define CWD_BUFFER_SIZE 4096


static i32 __Command_TaskList_Impl(
        usize workspaceId,
        Arena* arena)
{
    Client client = globalClientContext.client;

    Endpoint_TaskList_Request request = {
            .workspaceId = workspaceId,
    };
    Endpoint_TaskList_Response response = { 0 };

    ReturnCode returnCode = Endpoint_TaskList_Call(&client, &request, &response, arena);

    if (returnCode != RETURN_CODE_OK) {
        fprintf(stderr,
                "Task list failed with status code: %d\n",
                (i32)returnCode);
        return (i32)returnCode;
    }

    Str idHeader   = Str_FromCStr("ID");
    Str nameHeader = Str_FromCStr("Name");
    Str pathHeader = Str_FromCStr("Path");

    Str* idValues   = (Str*)Arena_Alloc(arena, response.taskIds.len * sizeof(Str));
    Str* nameValues = response.taskNames.data;
    Str* pathValues = response.taskPaths.data;

    usize maxIdLen   = idHeader.len;
    usize maxNameLen = nameHeader.len;
    usize maxPathLen = pathHeader.len;

    for (usize i = 0; i < response.taskIds.len; i++) {
        idValues[i] = Str_FromU64(response.taskIds.data[i], arena);

        if (idValues[i].len > maxIdLen) {
            maxIdLen = idValues[i].len;
        }
        if (nameValues[i].len > maxNameLen) {
            maxNameLen = nameValues[i].len;
        }
        if (pathValues[i].len > maxPathLen) {
            maxPathLen = pathValues[i].len;
        }
    }

    printf("%-*s  %-*s  %-*s\n", (i32)maxIdLen, "ID",
            (i32)maxNameLen, "Name", (i32)maxPathLen, "Path");

    for (usize i = 0; i < maxIdLen; i++) {
        putchar('-');
    }
    printf("  ");
    for (usize i = 0; i < maxNameLen; i++) {
        putchar('-');
    }
    printf("  ");
    for (usize i = 0; i < maxPathLen; i++) {
        putchar('-');
    }
    printf("\n");

    for (usize i = 0; i < response.taskIds.len; i++) {
        printf("%-*.*s  %-*.*s  %-*.*s\n",
                (i32)maxIdLen, (i32)idValues[i].len, idValues[i].data,
                (i32)maxNameLen, (i32)nameValues[i].len, nameValues[i].data,
                (i32)maxPathLen, (i32)pathValues[i].len, pathValues[i].data);
    }

    return 0;
}

static i32 __Command_TaskList_Handler(const Args* args, Arena* arena)
{
    assert(args->len == 1);

    usize workspaceId = 0;
    if (args->args[0].initialized) {
        workspaceId = (usize)strtoull(args->args[0].value.data, NULL, 10);
    } else {
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

        workspaceId = config.workspaceId;
    }

    return __Command_TaskList_Impl(workspaceId, arena);
}

Command Command_TaskList_Create(Arena* arena)
{
    Args args = {
        .len = 1,
    };

    args.args[0] = (Arg) {
        .shortForm    = Str_FromCStr("-w"),
        .fullForm     = Str_FromCStr("--workspace"),
        .required     = FALSE,
        .position     = ARG_POSITION_NONE,
        .value        = Str_CreateEmpty(),
        .initialized  = FALSE,
    };

    Str* prefixData = (Str*)Arena_Alloc(arena, sizeof(Str) * 2);
    prefixData[0] = Str_FromCStr("task");
    prefixData[1] = Str_FromCStr("list");
    SliceStr prefix = SliceStr_FromData(prefixData, 2);

    Command cmd = {
        .prefix  = prefix,
        .args    = args,
        .handler = __Command_TaskList_Handler,
    };

    return cmd;
}
