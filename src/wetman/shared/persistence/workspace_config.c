#include <wetman/shared/persistence/workspace_config.h>

#include <wetman/utils/data_stream.h>
#include <wetman/utils/filesystem.h>

#include <fcntl.h>
#include <unistd.h>


WorkspaceConfig WorkspaceConfig_Read(
        Str                filePath,
        Arena*             arena,
        PersistenceStatus* status)
{
    WorkspaceConfig config = {0};

    i32 fd = FS_OpenFile(filePath, O_RDONLY | O_EXLOCK);
    if (fd == -1) {
        *status = PERSISTENCE_STATUS_FILE_DOES_NOT_EXIST;
        return config;
    }
    // TODO: Allow read without setting max len
    DataStream dataStream = DataStream_Read(fd, arena, 4096);

    close(fd);


    config.workspaceId   = DataStream_PopU64(&dataStream);
    config.workspaceName = DataStream_PopStr(&dataStream);
    if (dataStream.lastResult != DATA_STREAM_RESULT_SUCCESS) {
        *status = PERSISTENCE_STATUS_WRONG_FORMAT;
        return config;
    }

    *status = PERSISTENCE_STATUS_OK;
    return config;
}

PersistenceStatus WorkspaceConfig_Write(
        Str                    filePath,
        const WorkspaceConfig* config)
{
    i32 fd = FS_OpenFile(filePath, O_WRONLY | O_TRUNC | O_EXLOCK);
    if (fd == -1) {
        return PERSISTENCE_STATUS_FILE_DOES_NOT_EXIST;
    }

    Arena arena = Arena_New();
    DataStream dataStream = DataStream_New();

    DataStream_PushU64(&dataStream, config->workspaceId, &arena);
    DataStream_PushStr(&dataStream, config->workspaceName, &arena);

    DataStream_Write(&dataStream, fd);

    close(fd);
    Arena_Free(&arena);

    return PERSISTENCE_STATUS_OK;
}
