#include <wetman/server/context.h>

#include <wetman/utils/filesystem.h>
#include <wetman/utils/macro.h>

#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>


#define WETMAN_NEXT_WORKSPACE_ID_FILE "next_workspace_id"

static pthread_mutex_t __nextWorkspaceIdMutex = PTHREAD_MUTEX_INITIALIZER;

static void __ServerContext_InitNextWorkspaceId(void)
{
    FS_CreateDir(globalServerContext.wdir);

    if (!FS_CheckExists(globalServerContext.nextWorkspaceIdPath)) {
        i32 fd = FS_OpenFile(
                globalServerContext.nextWorkspaceIdPath,
                O_WRONLY | O_CREAT | O_EXCL);
        if (fd != -1) {
            const u64 initialValue = 0;
            MAYBE_UNUSED isize bytesWritten =
                    write(fd, &initialValue, sizeof(initialValue));
            close(fd);
        }
    }

    i32 fd = FS_OpenFile(globalServerContext.nextWorkspaceIdPath, O_RDONLY);
    if (fd == -1) {
        return;
    }

    u64 storedValue = 0;
    isize bytesRead = read(fd, &storedValue, sizeof(storedValue));
    if (bytesRead == (isize)sizeof(storedValue)) {
        globalServerContext.nextWorkspaceId = (usize)storedValue;
    }

    close(fd);
}

static usize __ServerContext_NextWorkspaceId(void)
{
    pthread_mutex_lock(&__nextWorkspaceIdMutex);

    const usize workspaceId = globalServerContext.nextWorkspaceId++;

    i32 fd = FS_OpenFile(globalServerContext.nextWorkspaceIdPath, O_WRONLY);
    if (LIKELY(fd != -1)) {
        const u64 nextValue = (u64)(workspaceId + 1);
        MAYBE_UNUSED isize bytesWritten =
                write(fd, &nextValue, sizeof(nextValue));
        close(fd);
    }

    pthread_mutex_unlock(&__nextWorkspaceIdMutex);

    return workspaceId;
}

void ServerContext_Init(Arena arena, Str wdir)
{
    Str workspacesPath = FS_PathJoin(wdir, Str_FromCStr("workspaces"), &arena);
    Str nextWorkspaceIdPath = FS_PathJoin(
            wdir,
            Str_FromCStr(WETMAN_NEXT_WORKSPACE_ID_FILE),
            &arena);

    globalServerContext = (ServerContext) {
        .arena               = arena,
        .wdir                = wdir,
        .workspacesPath      = workspacesPath,
        .nextWorkspaceIdPath = nextWorkspaceIdPath,
        .nextWorkspaceId     = 0,
    };

    __ServerContext_InitNextWorkspaceId();
}

void ServerContext_Destroy(void)
{
    Arena_Free(&globalServerContext.arena);
}

usize ServerContext_InitWorkspace(Str workspaceConfigPath, Arena* arena)
{
    const usize workspaceId = __ServerContext_NextWorkspaceId();

    Str workspaceDirName = Str_FromU64((u64)workspaceId, arena);
    Str workspaceDir = FS_PathJoin(
            globalServerContext.workspacesPath,
            workspaceDirName,
            arena);
    FS_CreateDir(workspaceDir);

    Str configLinkPath = FS_PathJoin(
            workspaceDir,
            Str_FromCStr("workspace.wmwscfg"),
            arena);
    MAYBE_UNUSED i32 symlinkResult =
            FS_CreateSymlink(workspaceConfigPath, configLinkPath);

    return workspaceId;
}

ServerContext globalServerContext = {0};
