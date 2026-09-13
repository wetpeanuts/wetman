#include <wetman/client/commands/task_print.h>

#include <wetman/client/context.h>
#include <wetman/client/endpoint/task_get.h>
#include <wetman/shared/persistence/workspace_config.h>
#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/filesystem.h>

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


#define CWD_BUFFER_SIZE 4096


static i32 __Command_TaskPrint_Impl(
        usize workspaceId,
        usize taskId,
        Arena* arena)
{
    Client client = globalClientContext.client;

    Endpoint_TaskGet_Request request = {
            .workspaceId = workspaceId,
            .taskId      = taskId,
    };
    Endpoint_TaskGet_Response response = { 0 };

    ReturnCode returnCode = Endpoint_TaskGet_Call(&client, &request, &response, arena);

    if (returnCode != RETURN_CODE_OK) {
        fprintf(stderr,
                "Task print failed with status code: %d\n",
                (i32)returnCode);
        return (i32)returnCode;
    }

    i32 fd = FS_OpenFile(response.taskPath, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open task file\n");
        return 1;
    }

    struct stat st;
    if (fstat(fd, &st) != 0) {
        fprintf(stderr, "Failed to get task file stats\n");
        close(fd);
        return 1;
    }

    usize fileSize = (usize)st.st_size;
    char* content = (char*)Arena_Alloc(arena, fileSize);

    usize offset = 0;
    while (offset < fileSize) {
        isize bytesRead = read(fd, content + offset, fileSize - offset);
        if (bytesRead == -1) {
            fprintf(stderr, "Failed to read task file\n");
            close(fd);
            return 1;
        }
        if (bytesRead == 0) {
            break;
        }
        offset += (usize)bytesRead;
    }

    close(fd);

    printf("%.*s", (i32)offset, content);

    return 0;
}

static i32 __Command_TaskPrint_Handler(const Args* args, Arena* arena)
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

    return __Command_TaskPrint_Impl(workspaceId, taskId, arena);
}

Command Command_TaskPrint_Create(Arena* arena)
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
    prefixData[1] = Str_FromCStr("print");
    SliceStr prefix = SliceStr_FromData(prefixData, 2);

    Command cmd = {
        .prefix  = prefix,
        .args    = args,
        .handler = __Command_TaskPrint_Handler,
    };

    return cmd;
}
