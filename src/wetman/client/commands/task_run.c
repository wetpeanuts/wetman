#include <wetman/client/commands/task_run.h>

#include <wetman/client/context.h>
#include <wetman/client/endpoint/task_run.h>
#include <wetman/shared/persistence/workspace_config.h>
#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/filesystem.h>
#include <wetman/utils/macro.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define CWD_BUFFER_SIZE 4096


static i32 __Command_TaskRun_Impl(
        usize workspaceId,
        usize taskId,
        Arena* arena)
{
    Client client = globalClientContext.client;

    Endpoint_TaskRun_Request request = {
            .workspaceId = workspaceId,
            .taskId      = taskId,
    };
    Endpoint_TaskRun_Response response = {
            .stdoutFd = FileDescriptor_New(FILE_DESCRIPTOR_INVALID),
    };

    ReturnCode returnCode = Endpoint_TaskRun_Call(&client, &request, &response, arena);

    if (returnCode != RETURN_CODE_OK) {
        fprintf(stderr,
                "Task run failed with status code: %d\n",
                (i32)returnCode);
        return (i32)returnCode;
    }

    if (response.stdoutFd.fd == FILE_DESCRIPTOR_INVALID) {
        fprintf(stderr, "Task run returned no output stream\n");
        return 1;
    }

    char buf[4096];
    isize bytesRead;
    while ((bytesRead = read(response.stdoutFd.fd, buf, sizeof(buf))) > 0) {
        MAYBE_UNUSED isize written = fwrite(buf, 1, (usize)bytesRead, stdout);
    }
    close(response.stdoutFd.fd);

    return 0;
}

static i32 __Command_TaskRun_Handler(const Args* args, Arena* arena)
{
    assert(args->len == 2);

    usize taskId = (usize)strtoull(args->args[0].value.data, NULL, 10);

    usize workspaceId = 0;
    if (args->args[1].initialized) {
        workspaceId = (usize)strtoull(args->args[1].value.data, NULL, 10);
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

    return __Command_TaskRun_Impl(workspaceId, taskId, arena);
}

Command Command_TaskRun_Create(Arena* arena)
{
    Args args = {
        .len = 2,
    };

    args.args[0] = (Arg) {
        .shortForm    = Str_CreateEmpty(),
        .fullForm     = Str_CreateEmpty(),
        .required     = TRUE,
        .position     = 0,
        .value        = Str_CreateEmpty(),
        .initialized  = FALSE,
    };

    args.args[1] = (Arg) {
        .shortForm    = Str_FromCStr("-w"),
        .fullForm     = Str_FromCStr("--workspace"),
        .required     = FALSE,
        .position     = ARG_POSITION_NONE,
        .value        = Str_CreateEmpty(),
        .initialized  = FALSE,
    };

    Str* prefixData = (Str*)Arena_Alloc(arena, sizeof(Str) * 2);
    prefixData[0] = Str_FromCStr("task");
    prefixData[1] = Str_FromCStr("run");
    SliceStr prefix = SliceStr_FromData(prefixData, 2);

    Command cmd = {
        .prefix  = prefix,
        .args    = args,
        .handler = __Command_TaskRun_Handler,
    };

    return cmd;
}
