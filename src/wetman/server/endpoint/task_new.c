#include <wetman/server/endpoint/task_new.h>

#include <wetman/server/context.h>
#include <wetman/server/persistence/task_state.h>
#include <wetman/shared/endpoint/id.h>
#include <wetman/shared/persistence/workspace_config.h>

#include <wetman/utils/net/macro.h>
#include <wetman/utils/filesystem.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>


static Str __Endpoint_TaskNew_BuildShellScript(
        usize taskId,
        Str   taskName,
        Str   taskWdir,
        Arena* arena)
{
    Str content = Str_FromCStr("echo \"Uninitialized task\\n  id:   ");
    content = Str_Concat(content, Str_FromU64((u64)taskId, arena), arena);
    content = Str_Concat(content, Str_FromCStr("\\n  name: "), arena);
    content = Str_Concat(content, taskName, arena);
    content = Str_Concat(content, Str_FromCStr("\\n  wdir: "), arena);
    content = Str_Concat(content, taskWdir, arena);
    content = Str_Concat(content, Str_FromCStr("\\n\""), arena);

    return content;
}


ReturnCode Endpoint_TaskNew(
        Endpoint_TaskNew_Request*  request,
        Endpoint_TaskNew_Response* response)
{
    Arena arena = Arena_New();

    Str workspaceDirName = Str_FromU64((u64)request->workspaceId, &arena);
    Str workspaceDir = FS_PathJoin(
            globalServerContext.workspacesPath,
            workspaceDirName,
            &arena);

    if (!FS_CheckExists(workspaceDir)) {
        Arena_Free(&arena);
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    Str configPath = FS_PathJoin(
            workspaceDir,
            Str_FromCStr("workspace.wmwscfg"),
            &arena);

    PersistenceStatus status;
    WorkspaceConfig config = WorkspaceConfig_Read(configPath, &arena, &status);
    if (status != PERSISTENCE_STATUS_OK) {
        Arena_Free(&arena);
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    const usize taskId = config.nextTaskId;
    response->taskId = taskId;

    Str taskName = Str_Concat(Str_FromCStr(""), request->taskName, &arena);

    Str tasksDir = FS_PathJoin(
            workspaceDir,
            Str_FromCStr("tasks"),
            &arena);
    FS_CreateDir(tasksDir);

    Str taskDirName = Str_FromU64((u64)taskId, &arena);
    Str taskDir = FS_PathJoin(tasksDir, taskDirName, &arena);
    if (!FS_CreateDir(taskDir)) {
        Arena_Free(&arena);
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    Str taskShPath = FS_PathJoin(taskDir, Str_FromCStr("task.sh"), &arena);
    i32 fdTaskSh = FS_OpenFile(taskShPath, O_WRONLY | O_CREAT | O_TRUNC);
    if (fdTaskSh == -1) {
        Arena_Free(&arena);
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }
    fchmod(fdTaskSh, S_IRWXU);

    Str taskShContent = __Endpoint_TaskNew_BuildShellScript(
            taskId,
            taskName,
            config.workspacePath,
            &arena);
    MAYBE_UNUSED isize bytesWritten =
            write(fdTaskSh, taskShContent.data, taskShContent.len);
    close(fdTaskSh);

    Str taskStatePath = FS_PathJoin(taskDir, Str_FromCStr("task.wmtsk"), &arena);
    i32 fdTaskState = FS_OpenFile(taskStatePath, O_WRONLY | O_CREAT | O_TRUNC);
    if (fdTaskState == -1) {
        Arena_Free(&arena);
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }
    close(fdTaskState);

    TaskState taskState = {
        .taskId   = taskId,
        .taskName = taskName,
    };
    status = TaskState_Write(taskStatePath, &taskState);
    if (status != PERSISTENCE_STATUS_OK) {
        Arena_Free(&arena);
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    config.nextTaskId = taskId + 1;
    status = WorkspaceConfig_Write(configPath, &config);
    if (status != PERSISTENCE_STATUS_OK) {
        Arena_Free(&arena);
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    Arena_Free(&arena);

    return RETURN_CODE_OK;
}

ENDPOINT_IMPL_SERVER(ENDPOINT_ID_TASK_NEW, Endpoint_TaskNew)

