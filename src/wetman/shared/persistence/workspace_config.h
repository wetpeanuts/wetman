#ifndef WETMAN_SHARED_PERSISTENCE_WORKSPACE_CONFIG_H
#define WETMAN_SHARED_PERSISTENCE_WORKSPACE_CONFIG_H

#include <wetman/shared/persistence/status.h>
#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/mem/arena.h>


typedef struct {
    usize             workspaceId;
    Str               workspaceName;
    Str               workspacePath;
} WorkspaceConfig;


WorkspaceConfig WorkspaceConfig_Read(
        Str                filePath,
        Arena*             arena,
        PersistenceStatus* status);

PersistenceStatus WorkspaceConfig_Write(
        Str                    filePath,
        const WorkspaceConfig* config);

#endif // WETMAN_SHARED_PERSISTENCE_WORKSPACE_CONFIG_H
