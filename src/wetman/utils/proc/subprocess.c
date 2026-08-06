#include <wetman/utils/proc/subprocess.h>

#include <stdio.h>


pid_t Subprocess_Run(Callback callback)
{
    pid_t pid = fork();

    if (pid == -1) {
        // TODO: do not exit of failed fork, let user handle
        perror("Error: failed to fork subprocess");
        exit(1);
    }

    if (pid == 0) {
        Callback_Invoke(&callback);
        exit(0);
    }

    return pid;
}
