#include <wetman/server/endpoint/task_delete.h>

#include <wetman/server/context.h>
#include <wetman/shared/endpoint/id.h>

#include <wetman/utils/net/macro.h>
#include <wetman/utils/filesystem.h>

#include <limits.h>
#include <stdio.h>


ReturnCode Endpoint_TaskDelete(
        Endpoint_TaskDelete_Request*  request,
        Endpoint_TaskDelete_Response* response,
        Arena*                        arena)
{
    Str workspaceDirName = Str_FromU64((u64)request->workspaceId, arena);
    Str workspaceDir = FS_PathJoin(
            globalServerContext.workspacesPath,
            workspaceDirName,
            arena);

    if (!FS_CheckExists(workspaceDir)) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    Str tasksDir = FS_PathJoin(
            workspaceDir,
            Str_FromCStr("tasks"),
            arena);
    Str taskDirName = Str_FromU64((u64)request->taskId, arena);
    Str taskDir = FS_PathJoin(tasksDir, taskDirName, arena);

    if (!FS_CheckExists(taskDir)) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    char taskDirBuf[PATH_MAX];
    i32 n = snprintf(
            taskDirBuf,
            sizeof(taskDirBuf),
            "%.*s",
            (i32)taskDir.len,
            taskDir.data);
    if (n < 0 || (usize)n >= sizeof(taskDirBuf)) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    if (FS_RemoveDirectoryRecursive(taskDirBuf) == -1) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    response->taskId = request->taskId;

    return RETURN_CODE_OK;
}

ENDPOINT_IMPL_SERVER(ENDPOINT_ID_TASK_DELETE, Endpoint_TaskDelete)
