#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <wetman/shared/persistence/workspace_config.h>
#include <wetman/utils/filesystem.h>
#include <wetman/utils/test/macro.h>


TEST(WorkspaceConfigTest_WriteRead_Success)
{
    Arena arena = Arena_New();

    char tmpDir[256];
    CREATE_TMP_DIR(tmpDir);

    Str configPath = FS_PathJoin(
            Str_FromCStr(tmpDir),
            Str_FromCStr("workspace.wmwscfg"),
            &arena);

    i32 fd = FS_OpenFile(configPath, O_WRONLY | O_CREAT | O_TRUNC);
    ASSERT_NE(fd, -1);
    close(fd);

    WorkspaceConfig config = {
            .workspaceId   = 42,
            .workspaceName = Str_FromCStr("test_workspace"),
            .nextTaskId    = 7,
    };

    PersistenceStatus writeStatus = WorkspaceConfig_Write(configPath, &config);
    ASSERT_EQ(writeStatus, PERSISTENCE_STATUS_OK);
    ASSERT(FS_CheckExists(configPath));

    PersistenceStatus readStatus = PERSISTENCE_STATUS_OK;
    WorkspaceConfig readConfig = WorkspaceConfig_Read(configPath, &arena, &readStatus);

    ASSERT_EQ(readStatus, PERSISTENCE_STATUS_OK);
    EXPECT_EQ(readConfig.workspaceId, 42);
    EXPECT(Str_EqCStr(readConfig.workspaceName, "test_workspace"));
    EXPECT_EQ(readConfig.nextTaskId, 7);

    Arena_Free(&arena);
}

TEST(WorkspaceConfigTest_Write_FileDoesNotExist)
{
    Arena arena = Arena_New();

    char tmpDir[256];
    CREATE_TMP_DIR(tmpDir);

    Str configPath = FS_PathJoin(
            Str_FromCStr(tmpDir),
            Str_FromCStr("workspace.wmwscfg"),
            &arena);

    ASSERT(!FS_CheckExists(configPath));

    WorkspaceConfig config = {
            .workspaceId   = 1,
            .workspaceName = Str_FromCStr("test_workspace"),
    };

    PersistenceStatus status = WorkspaceConfig_Write(configPath, &config);

    EXPECT_EQ(status, PERSISTENCE_STATUS_FILE_DOES_NOT_EXIST);

    Arena_Free(&arena);
}

TEST(WorkspaceConfigTest_Read_FileDoesNotExist)
{
    Arena arena = Arena_New();

    char tmpDir[256];
    CREATE_TMP_DIR(tmpDir);

    Str configPath = FS_PathJoin(
            Str_FromCStr(tmpDir),
            Str_FromCStr("workspace.wmwscfg"),
            &arena);

    ASSERT(!FS_CheckExists(configPath));

    PersistenceStatus status = PERSISTENCE_STATUS_OK;
    WorkspaceConfig config = WorkspaceConfig_Read(configPath, &arena, &status);

    EXPECT_EQ(status, PERSISTENCE_STATUS_FILE_DOES_NOT_EXIST);
    EXPECT_EQ(config.workspaceId, 0);

    Arena_Free(&arena);
}

TEST(WorkspaceConfigTest_Read_WrongFormat)
{
    Arena arena = Arena_New();

    char tmpDir[256];
    CREATE_TMP_DIR(tmpDir);

    Str configPath = FS_PathJoin(
            Str_FromCStr(tmpDir),
            Str_FromCStr("workspace.wmwscfg"),
            &arena);

    i32 fd = FS_OpenFile(configPath, O_WRONLY | O_CREAT | O_TRUNC);
    ASSERT_NE(fd, -1);

    const char* garbage = "this is definitely not a valid workspace config";
    usize garbageLen = strlen(garbage);
    ASSERT_EQ(write(fd, garbage, garbageLen), (long long)garbageLen);
    close(fd);

    PersistenceStatus status = PERSISTENCE_STATUS_OK;
    MAYBE_UNUSED WorkspaceConfig config = WorkspaceConfig_Read(configPath, &arena, &status);

    EXPECT_EQ(status, PERSISTENCE_STATUS_WRONG_FORMAT);

    Arena_Free(&arena);
}

