#ifndef WETMAN_SERVER_CONTEXT_H
#define WETMAN_SERVER_CONTEXT_H

#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/mem/arena.h>


typedef struct {
    Arena arena;
    Str   wdir;
    Str   workspacesPath;
    Str   nextWorkspaceIdPath;
    usize nextWorkspaceId;
} ServerContext;


void ServerContext_Init(Arena arena, Str wdir);
void ServerContext_Destroy(void);

usize ServerContext_InitWorkspace(Str workspaceConfigPath, Arena* arena);

extern ServerContext globalServerContext;

#endif // WETMAN_SERVER_CONTEXT_H

