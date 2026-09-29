#include <wetman/utils/proc/signal.h>

#include <string.h>


static volatile sig_atomic_t __shutdownRequested = 0;
static volatile sig_atomic_t __receivedSignal     = 0;


static void Signal_ShutdownHandler(int signum)
{
    // Async-signal-safe: sig_atomic_t assignments only, no stdio, no locks.
    __receivedSignal     = signum;
    __shutdownRequested = 1;
}

void Signal_InstallShutdownHandler(int signum)
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = Signal_ShutdownHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(signum, &action, NULL);
}

void Signal_RequestShutdown(int signum)
{
    __receivedSignal     = signum;
    __shutdownRequested = 1;
}

int Signal_ShutdownRequested(void)
{
    return __shutdownRequested != 0;
}

int Signal_ReceivedSignal(void)
{
    return (int)__receivedSignal;
}
