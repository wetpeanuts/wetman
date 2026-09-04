#include <wetman/server/endpoint/workspace_list.h>

#include <wetman/server/context.h>
#include <wetman/shared/endpoint/id.h>
#include <wetman/shared/persistence/workspace_config.h>

#include <wetman/utils/net/macro.h>
#include <wetman/utils/filesystem.h>
#include <wetman/utils/data_struct/vec.h>


ReturnCode Endpoint_WorkspaceList(
        Endpoint_WorkspaceList_Request*  request,
        Endpoint_WorkspaceList_Response* response)
{
    (void)request;

    Arena* arena = &globalServerContext.arena;

    // TODO: Init vec with capacity
    Vec ids    = VEC_NEW(u64, arena);
    Vec names  = VEC_NEW(Str, arena);
    Vec paths  = VEC_NEW(Str, arena);

    // TODO: Walk only existing dirs in file system
    for (usize workspaceId = 0;
            workspaceId < globalServerContext.nextWorkspaceId;
            workspaceId++) {
        Str workspaceDirName = Str_FromU64((u64)workspaceId, arena);
        Str workspaceDir = FS_PathJoin(
                globalServerContext.workspacesPath,
                workspaceDirName,
                arena);

        if (!FS_CheckExists(workspaceDir)) {
            continue;
        }

        Str configPath = FS_PathJoin(
                workspaceDir,
                Str_FromCStr("workspace.wmwscfg"), // TODO: put the const in globalServerContext
                arena);

        PersistenceStatus status;
        WorkspaceConfig config = WorkspaceConfig_Read(configPath, arena, &status);
        if (status != PERSISTENCE_STATUS_OK) {
            continue;
        }

        u64 id = (u64)config.workspaceId;
        VEC_PUSH(u64, &ids, &id);
        VEC_PUSH(Str, &names, &config.workspaceName);
        VEC_PUSH(Str, &paths, &config.workspacePath);
    }

    response->workspaceIds = SliceU64_FromData((u64*)ids.__data, ids.len);
    response->workspaceNames = SliceStr_FromData((Str*)names.__data, names.len);
    response->workspacePaths = SliceStr_FromData((Str*)paths.__data, paths.len);

    return RETURN_CODE_OK;
}

ENDPOINT_IMPL_SERVER(ENDPOINT_ID_WORKSPACE_LIST, Endpoint_WorkspaceList)
