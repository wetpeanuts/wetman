#include <wetman/server/endpoint/workspace_delete.h>

#include <wetman/server/context.h>
#include <wetman/shared/endpoint/id.h>
#include <wetman/shared/persistence/workspace_config.h>

#include <wetman/utils/net/macro.h>
#include <wetman/utils/filesystem.h>

#include <limits.h>
#include <stdio.h>


ReturnCode Endpoint_WorkspaceDelete(
        Endpoint_WorkspaceDelete_Request*  request,
        Endpoint_WorkspaceDelete_Response* response)
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

    char wetmanDirBuf[PATH_MAX];
    i32 n = snprintf(
            wetmanDirBuf,
            sizeof(wetmanDirBuf),
            "%.*s/.wetman",
            (i32)config.workspacePath.len,
            config.workspacePath.data);
    if (n < 0 || (usize)n >= sizeof(wetmanDirBuf)) {
        Arena_Free(&arena);
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    if (FS_RemoveDirectoryRecursive(wetmanDirBuf) == -1) {
        Arena_Free(&arena);
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    char workspaceDirBuf[PATH_MAX];
    n = snprintf(
            workspaceDirBuf,
            sizeof(workspaceDirBuf),
            "%.*s",
            (i32)workspaceDir.len,
            workspaceDir.data);
    if (n < 0 || (usize)n >= sizeof(workspaceDirBuf)) {
        Arena_Free(&arena);
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    if (FS_RemoveDirectoryRecursive(workspaceDirBuf) == -1) {
        Arena_Free(&arena);
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    response->workspaceId = request->workspaceId;

    Arena_Free(&arena);

    return RETURN_CODE_OK;
}

ENDPOINT_IMPL_SERVER(ENDPOINT_ID_WORKSPACE_DELETE, Endpoint_WorkspaceDelete)
