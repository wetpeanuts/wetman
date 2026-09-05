#include <wetman/server/persistence/task_state.h>

#include <wetman/utils/data_stream.h>
#include <wetman/utils/filesystem.h>

#include <fcntl.h>
#include <unistd.h>


TaskState TaskState_Read(
        Str                filePath,
        Arena*             arena,
        PersistenceStatus* status)
{
    TaskState taskState = {0};

    i32 fd = FS_OpenFile(filePath, O_RDONLY | O_EXLOCK);
    if (fd == -1) {
        *status = PERSISTENCE_STATUS_FILE_DOES_NOT_EXIST;
        return taskState;
    }
    // TODO: Allow read without setting max len
    DataStream dataStream = DataStream_Read(fd, arena, 4096);

    close(fd);


    taskState.taskId   = DataStream_PopU64(&dataStream);
    taskState.taskName = DataStream_PopStr(&dataStream);
    if (dataStream.lastResult != DATA_STREAM_RESULT_SUCCESS) {
        *status = PERSISTENCE_STATUS_WRONG_FORMAT;
        return taskState;
    }

    *status = PERSISTENCE_STATUS_OK;
    return taskState;
}

PersistenceStatus TaskState_Write(
        Str                filePath,
        const TaskState*   taskState)
{
    i32 fd = FS_OpenFile(filePath, O_WRONLY | O_TRUNC | O_EXLOCK);
    if (fd == -1) {
        return PERSISTENCE_STATUS_FILE_DOES_NOT_EXIST;
    }

    Arena arena = Arena_New();
    DataStream dataStream = DataStream_New();

    DataStream_PushU64(&dataStream, (u64)taskState->taskId, &arena);
    DataStream_PushStr(&dataStream, taskState->taskName, &arena);

    DataStream_Write(&dataStream, fd);

    close(fd);
    Arena_Free(&arena);

    return PERSISTENCE_STATUS_OK;
}

