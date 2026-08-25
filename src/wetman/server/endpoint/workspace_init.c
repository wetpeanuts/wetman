#include <wetman/server/endpoint/workspace_init.h>

#include <wetman/server/context.h>
#include <wetman/shared/endpoint/id.h>
#include <wetman/shared/persistence/workspace_config.h>

#include <wetman/utils/net/macro.h>
#include <wetman/utils/filesystem.h>

#include <fcntl.h>


ReturnCode Endpoint_WorkspaceInit(
        Endpoint_WorkspaceInit_Request*  request,
        Endpoint_WorkspaceInit_Response* response)
{
    if (!FS_CheckExists(request->workspacePath)) {
        // Project dir does not exist
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    Arena arena = Arena_New();
    Str workspaceDir = Str_FromCStr(".wetman");
    Str workspacePath = FS_PathJoin(
            request->workspacePath,
            workspaceDir,
            &arena);

    if (FS_CheckExists(workspacePath)) {
        // Workspace already exists
        Arena_Free(&arena);
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    if (!FS_CreateDir(workspacePath)) {
        // Failed to create workspace
        Arena_Free(&arena);
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    Str workspaceConfigPath = FS_PathJoin(
            workspacePath,
            Str_FromCStr("workspace.wmwscfg"),
            &arena);
    const i32 fdWorkspaceConfig = FS_OpenFile(
            workspaceConfigPath, O_WRONLY | O_CREAT | O_EXCL);
    if (fdWorkspaceConfig == -1) {
        // Failed to create workspace config
        Arena_Free(&arena);
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }
    close(fdWorkspaceConfig);

    response->workspaceId =
        ServerContext_InitWorkspace(workspaceConfigPath, &arena);

    WorkspaceConfig config = {
        .workspaceId   = response->workspaceId,
        .workspaceName = request->workspaceName,
        .workspacePath = request->workspacePath,
    };

    PersistenceStatus status = WorkspaceConfig_Write(workspaceConfigPath, &config);
    if (status != PERSISTENCE_STATUS_OK) {
        Arena_Free(&arena);
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    Arena_Free(&arena);

    return RETURN_CODE_OK;
}

ENDPOINT_IMPL_SERVER(ENDPOINT_ID_WORKSPACE_INIT, Endpoint_WorkspaceInit)
