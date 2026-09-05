#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <wetman/server/persistence/task_state.h>
#include <wetman/utils/filesystem.h>
#include <wetman/utils/test/macro.h>


TEST(TaskStateTest_WriteRead_Success)
{
    Arena arena = Arena_New();

    char tmpDir[256];
    CREATE_TMP_DIR(tmpDir);

    Str taskStatePath = FS_PathJoin(
            Str_FromCStr(tmpDir),
            Str_FromCStr("task.wmtsk"),
            &arena);

    i32 fd = FS_OpenFile(taskStatePath, O_WRONLY | O_CREAT | O_TRUNC);
    ASSERT_NE(fd, -1);
    close(fd);

    TaskState taskState = {
            .taskId   = 7,
            .taskName = Str_FromCStr("test_task"),
    };

    PersistenceStatus writeStatus = TaskState_Write(taskStatePath, &taskState);
    ASSERT_EQ(writeStatus, PERSISTENCE_STATUS_OK);
    ASSERT(FS_CheckExists(taskStatePath));

    PersistenceStatus readStatus = PERSISTENCE_STATUS_OK;
    TaskState readTaskState = TaskState_Read(taskStatePath, &arena, &readStatus);

    ASSERT_EQ(readStatus, PERSISTENCE_STATUS_OK);
    EXPECT_EQ(readTaskState.taskId, 7);
    EXPECT(Str_EqCStr(readTaskState.taskName, "test_task"));

    Arena_Free(&arena);
}

TEST(TaskStateTest_Write_FileDoesNotExist)
{
    Arena arena = Arena_New();

    char tmpDir[256];
    CREATE_TMP_DIR(tmpDir);

    Str taskStatePath = FS_PathJoin(
            Str_FromCStr(tmpDir),
            Str_FromCStr("task.wmtsk"),
            &arena);

    ASSERT(!FS_CheckExists(taskStatePath));

    TaskState taskState = {
            .taskId   = 1,
            .taskName = Str_FromCStr("test_task"),
    };

    PersistenceStatus status = TaskState_Write(taskStatePath, &taskState);

    EXPECT_EQ(status, PERSISTENCE_STATUS_FILE_DOES_NOT_EXIST);

    Arena_Free(&arena);
}

TEST(TaskStateTest_Read_FileDoesNotExist)
{
    Arena arena = Arena_New();

    char tmpDir[256];
    CREATE_TMP_DIR(tmpDir);

    Str taskStatePath = FS_PathJoin(
            Str_FromCStr(tmpDir),
            Str_FromCStr("task.wmtsk"),
            &arena);

    ASSERT(!FS_CheckExists(taskStatePath));

    PersistenceStatus status = PERSISTENCE_STATUS_OK;
    TaskState taskState = TaskState_Read(taskStatePath, &arena, &status);

    EXPECT_EQ(status, PERSISTENCE_STATUS_FILE_DOES_NOT_EXIST);
    EXPECT_EQ(taskState.taskId, 0);

    Arena_Free(&arena);
}

TEST(TaskStateTest_Read_WrongFormat)
{
    Arena arena = Arena_New();

    char tmpDir[256];
    CREATE_TMP_DIR(tmpDir);

    Str taskStatePath = FS_PathJoin(
            Str_FromCStr(tmpDir),
            Str_FromCStr("task.wmtsk"),
            &arena);

    i32 fd = FS_OpenFile(taskStatePath, O_WRONLY | O_CREAT | O_TRUNC);
    ASSERT_NE(fd, -1);

    const char* garbage = "this is definitely not a valid task state";
    usize garbageLen = strlen(garbage);
    ASSERT_EQ(write(fd, garbage, garbageLen), (long long)garbageLen);
    close(fd);

    PersistenceStatus status = PERSISTENCE_STATUS_OK;
    MAYBE_UNUSED TaskState taskState = TaskState_Read(taskStatePath, &arena, &status);

    EXPECT_EQ(status, PERSISTENCE_STATUS_WRONG_FORMAT);

    Arena_Free(&arena);
}

