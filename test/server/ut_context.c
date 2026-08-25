#include <wetman/server/context.h>
#include <wetman/utils/filesystem.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/test/macro.h>

#include <fcntl.h>


TEST(ServerContextTest_InitWorkspace_CreatesDirAndSymlink)
{
    Arena arena = Arena_New();

    char projectDir[256];
    CREATE_TMP_DIR(projectDir);

    char serverWorkingDir[256];
    CREATE_TMP_DIR(serverWorkingDir);

    ServerContext_Init(arena, Str_FromCStr(serverWorkingDir));

    Arena projectArena = Arena_New();
    Str projectPath = Str_FromCStr(projectDir);
    Str wetmanDir = FS_PathJoin(projectPath, Str_FromCStr(".wetman"), &projectArena);
    FS_CreateDir(wetmanDir);
    Str configPath = FS_PathJoin(wetmanDir, Str_FromCStr("workspace.wmwscfg"), &projectArena);
    i32 fd = FS_OpenFile(configPath, O_WRONLY | O_CREAT);
    ASSERT_NE(fd, -1);
    close(fd);

    const usize workspaceId = ServerContext_InitWorkspace(configPath, &arena);
    EXPECT_EQ(workspaceId, 0);

    Str workspaceDir = FS_PathJoin(
            globalServerContext.workspacesPath,
            Str_FromCStr("0"),
            &arena);
    EXPECT(FS_CheckExists(workspaceDir));

    Str configLinkPath = FS_PathJoin(
            workspaceDir,
            Str_FromCStr("workspace.wmwscfg"),
            &arena);
    EXPECT(FS_CheckExists(configLinkPath));

    const usize secondWorkspaceId =
            ServerContext_InitWorkspace(configPath, &arena);
    EXPECT_EQ(secondWorkspaceId, 1);

    Str secondWorkspaceDir = FS_PathJoin(
            globalServerContext.workspacesPath,
            Str_FromCStr("1"),
            &arena);
    EXPECT(FS_CheckExists(secondWorkspaceDir));

    ServerContext_Destroy();
    Arena_Free(&projectArena);
}
