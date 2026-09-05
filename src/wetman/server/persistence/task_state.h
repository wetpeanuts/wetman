#ifndef WETMAN_SERVER_PERSISTENCE_TASK_STATE_H
#define WETMAN_SERVER_PERSISTENCE_TASK_STATE_H

#include <wetman/shared/persistence/status.h>
#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/mem/arena.h>


typedef struct {
    usize taskId;
    Str   taskName;
} TaskState;


TaskState TaskState_Read(
        Str                filePath,
        Arena*             arena,
        PersistenceStatus* status);

PersistenceStatus TaskState_Write(
        Str                filePath,
        const TaskState*   taskState);

#endif // WETMAN_SERVER_PERSISTENCE_TASK_STATE_H

