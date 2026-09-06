#include <wetman/utils/proc/subprocess.h>

#include <stdio.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>


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

pid_t Subprocess_RunCommand(const char* command)
{
    pid_t pid = fork();

    if (pid == -1) {
        perror("Error: failed to fork subprocess");
        return -1;
    }

    if (pid == 0) {
        execlp("/bin/sh", "sh", "-c", command, (char*)NULL);
        perror("Error: failed to exec command");
        _exit(127);
    }

    return pid;
}

i32 Subprocess_WaitFor(pid_t processId, u64 milliseconds)
{
    struct timespec timeout;
    timeout.tv_sec = (time_t)(milliseconds / 1000);
    timeout.tv_nsec = (long)((milliseconds % 1000) * 1000000);

    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int status = -1;
    pid_t result;

    for (;;) {
        result = waitpid(processId, &status, WNOHANG);
        if (result == -1) {
            return -1;
        }
        if (result == processId) {
            return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
        }

        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);

        long elapsedSec = now.tv_sec - start.tv_sec;
        long elapsedNsec = now.tv_nsec - start.tv_nsec;
        if (elapsedNsec < 0) {
            elapsedSec -= 1;
            elapsedNsec += 1000000000L;
        }

        if (elapsedSec > timeout.tv_sec ||
            (elapsedSec == timeout.tv_sec && elapsedNsec > timeout.tv_nsec)) {
            return -1;
        }

        struct timespec sleep;
        sleep.tv_sec = 0;
        sleep.tv_nsec = 1000000;
        nanosleep(&sleep, NULL);
    }
}

i32 Subprocess_Kill(pid_t processId)
{
    if (0 == kill(processId, SIGTERM)) {
        return TRUE;
    }

    return FALSE;
}
