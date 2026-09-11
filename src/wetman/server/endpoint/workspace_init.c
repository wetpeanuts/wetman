#include <wetman/server/endpoint/workspace_init.h>

#include <wetman/server/context.h>
#include <wetman/shared/endpoint/id.h>
#include <wetman/shared/persistence/workspace_config.h>

#include <wetman/utils/net/macro.h>
#include <wetman/utils/filesystem.h>

#include <fcntl.h>


ReturnCode Endpoint_WorkspaceInit(
        Endpoint_WorkspaceInit_Request*  request,
        Endpoint_WorkspaceInit_Response* response,
        Arena*                           arena)
{
    if (!FS_CheckExists(request->workspacePath)) {
        // Project dir does not exist
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    Str workspaceDir = Str_FromCStr(".wetman");
    Str workspacePath = FS_PathJoin(
            request->workspacePath,
            workspaceDir,
            arena);

    if (FS_CheckExists(workspacePath)) {
        // Workspace already exists
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    if (!FS_CreateDir(workspacePath)) {
        // Failed to create workspace
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    Str workspaceConfigPath = FS_PathJoin(
            workspacePath,
            Str_FromCStr("workspace.wmwscfg"),
            arena);
    const i32 fdWorkspaceConfig = FS_OpenFile(
            workspaceConfigPath, O_WRONLY | O_CREAT | O_EXCL);
    if (fdWorkspaceConfig == -1) {
        // Failed to create workspace config
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }
    close(fdWorkspaceConfig);

response->workspaceId =
        ServerContext_InitWorkspace(workspaceConfigPath, arena);

    Str workspaceDirName = Str_FromU64((u64)response->workspaceId, arena);
    Str workspaceDirPath = FS_PathJoin(
            globalServerContext.workspacesPath,
            workspaceDirName,
            arena);
    Str tasksDir = FS_PathJoin(
            workspaceDirPath,
            Str_FromCStr("tasks"),
            arena);
    if (!FS_CreateDir(tasksDir)) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    WorkspaceConfig config = {
        .workspaceId   = response->workspaceId,
        .workspaceName = request->workspaceName,
        .workspacePath = request->workspacePath,
        .nextTaskId    = 0,
    };

    PersistenceStatus status = WorkspaceConfig_Write(workspaceConfigPath, &config);
    if (status != PERSISTENCE_STATUS_OK) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    return RETURN_CODE_OK;
}

ENDPOINT_IMPL_SERVER(ENDPOINT_ID_WORKSPACE_INIT, Endpoint_WorkspaceInit)
