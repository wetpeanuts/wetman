#ifndef WETMAN_UTILS_PROC_SUBPROCESS_H
#define WETMAN_UTILS_PROC_SUBPROCESS_H

#include <wetman/utils/async/callback.h>
#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/type.h>

#include <unistd.h>


pid_t Subprocess_Run(Callback callback);

// Runs bash command in a separate process
pid_t Subprocess_RunCommand(const char* command);

// Waits for process until timeout.
// Returns process return code if returned before timeout,
// returns -1 on timeout.
i32 Subprocess_WaitFor(pid_t processId, u64 milliseconds);
i32 Subprocess_Kill(pid_t processId);

#endif // WETMAN_UTILS_PROC_SUBPROCESS_H

