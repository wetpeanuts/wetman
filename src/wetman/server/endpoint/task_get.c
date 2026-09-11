#include <wetman/server/endpoint/task_get.h>

#include <wetman/server/context.h>
#include <wetman/server/persistence/task_state.h>
#include <wetman/shared/endpoint/id.h>

#include <wetman/utils/net/macro.h>
#include <wetman/utils/filesystem.h>


ReturnCode Endpoint_TaskGet(
        Endpoint_TaskGet_Request*  request,
        Endpoint_TaskGet_Response* response,
        Arena*                     arena)
{
    response->taskId   = 0;
    response->taskName = Str_FromCStr("");
    response->taskPath = Str_FromCStr("");

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

    Str taskStatePath = FS_PathJoin(taskDir, Str_FromCStr("task.wmtsk"), arena);

    PersistenceStatus status;
    TaskState taskState = TaskState_Read(taskStatePath, arena, &status);
    if (status != PERSISTENCE_STATUS_OK) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    Str taskShPath = FS_PathJoin(taskDir, Str_FromCStr("task.sh"), arena);
    if (!FS_CheckExists(taskShPath)) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    response->taskId   = taskState.taskId;
    response->taskName = Str_Concat(Str_FromCStr(""), taskState.taskName, arena);
    response->taskPath = Str_Concat(Str_FromCStr(""), taskShPath, arena);

    return RETURN_CODE_OK;
}

ENDPOINT_IMPL_SERVER(ENDPOINT_ID_TASK_GET, Endpoint_TaskGet)
