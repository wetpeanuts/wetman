#ifndef WETMAN_UTILS_PROC_SUBPROCESS_H
#define WETMAN_UTILS_PROC_SUBPROCESS_H

#include <wetman/utils/async/callback.h>

#include <unistd.h>


pid_t Subprocess_Run(Callback callback);

#endif // WETMAN_UTILS_PROC_SUBPROCESS_H

