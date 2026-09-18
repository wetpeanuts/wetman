#include <wetman/server/endpoint/task_list.h>

#include <wetman/server/context.h>
#include <wetman/server/persistence/task_state.h>
#include <wetman/shared/endpoint/id.h>
#include <wetman/shared/persistence/workspace_config.h>

#include <wetman/utils/net/macro.h>
#include <wetman/utils/filesystem.h>
#include <wetman/utils/data_struct/vec_u64.h>
#include <wetman/utils/data_struct/vec_str.h>


ReturnCode Endpoint_TaskList(
        Endpoint_TaskList_Request*  request,
        Endpoint_TaskList_Response* response,
        Arena*                      arena)
{
    response->taskIds   = SliceU64_CreateEmpty();
    response->taskNames = SliceStr_CreateEmpty();
    response->taskPaths = SliceStr_CreateEmpty();

    Str workspaceDirName = Str_FromU64((u64)request->workspaceId, arena);
    Str workspaceDir = FS_PathJoin(
            globalServerContext.workspacesPath,
            workspaceDirName,
            arena);

    if (!FS_CheckExists(workspaceDir)) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    Str configPath = FS_PathJoin(
            workspaceDir,
            Str_FromCStr("workspace.wmwscfg"), // TODO: put the const in globalServerContext
            arena);

    PersistenceStatus status;
    WorkspaceConfig config = WorkspaceConfig_Read(configPath, arena, &status);
    if (status != PERSISTENCE_STATUS_OK) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    VecU64 ids   = VecU64_WithCapacity(config.nextTaskId, arena);
    VecStr names = VecStr_WithCapacity(config.nextTaskId, arena);
    VecStr paths = VecStr_WithCapacity(config.nextTaskId, arena);

    Str tasksDir = FS_PathJoin(
            workspaceDir,
            Str_FromCStr("tasks"),
            arena);

    // TODO: Walk only existing dirs in file system
    for (usize taskId = 0; taskId < config.nextTaskId; taskId++) {
        Str taskDirName = Str_FromU64((u64)taskId, arena);
        Str taskDir = FS_PathJoin(tasksDir, taskDirName, arena);

        if (!FS_CheckExists(taskDir)) {
            continue;
        }

        Str taskStatePath = FS_PathJoin(taskDir, Str_FromCStr("task.wmtsk"), arena);

        TaskState taskState = TaskState_Read(taskStatePath, arena, &status);
        if (status != PERSISTENCE_STATUS_OK) {
            continue;
        }

        Str taskShPath = FS_PathJoin(taskDir, Str_FromCStr("task.sh"), arena);
        if (!FS_CheckExists(taskShPath)) {
            continue;
        }

        u64 id = (u64)taskState.taskId;
        VecU64_Push(&ids, &id);
        VecStr_Push(&names, &taskState.taskName);
        VecStr_Push(&paths, &taskShPath);
    }

    response->taskIds   = SliceU64_FromData(ids.data, ids.len);
    response->taskNames = SliceStr_FromData(names.data, names.len);
    response->taskPaths = SliceStr_FromData(paths.data, paths.len);

    return RETURN_CODE_OK;
}

ENDPOINT_IMPL_SERVER(ENDPOINT_ID_TASK_LIST, Endpoint_TaskList)
