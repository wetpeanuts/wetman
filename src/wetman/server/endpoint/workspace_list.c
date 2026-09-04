#include <wetman/server/endpoint/workspace_list.h>

#include <wetman/server/context.h>
#include <wetman/shared/endpoint/id.h>
#include <wetman/shared/persistence/workspace_config.h>

#include <wetman/utils/net/macro.h>
#include <wetman/utils/filesystem.h>
#include <wetman/utils/data_struct/vec_u64.h>
#include <wetman/utils/data_struct/vec_str.h>


ReturnCode Endpoint_WorkspaceList(
        Endpoint_WorkspaceList_Request*  request,
        Endpoint_WorkspaceList_Response* response)
{
    (void)request;

    Arena* arena = &globalServerContext.arena;

    VecU64 ids   = VecU64_WithCapacity(globalServerContext.nextWorkspaceId, arena);
    VecStr names = VecStr_WithCapacity(globalServerContext.nextWorkspaceId, arena);
    VecStr paths = VecStr_WithCapacity(globalServerContext.nextWorkspaceId, arena);

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
        VecU64_Push(&ids, &id);
        VecStr_Push(&names, &config.workspaceName);
        VecStr_Push(&paths, &config.workspacePath);
    }

    response->workspaceIds   = SliceU64_FromData(ids.data, ids.len);
    response->workspaceNames = SliceStr_FromData(names.data, names.len);
    response->workspacePaths = SliceStr_FromData(paths.data, paths.len);

    return RETURN_CODE_OK;
}

ENDPOINT_IMPL_SERVER(ENDPOINT_ID_WORKSPACE_LIST, Endpoint_WorkspaceList)
