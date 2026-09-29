#ifndef WETMAN_UTILS_PROC_SIGNAL_H
#define WETMAN_UTILS_PROC_SIGNAL_H

#include <wetman/utils/type.h>

#include <signal.h>


// Installs a handler that only requests a graceful shutdown. The handler is
// async-signal-safe, so the actual cleanup has to happen from the main flow.
// Deliberately omits SA_RESTART, otherwise blocking calls like poll() restart
// silently and the shutdown request is not noticed until the next event.
void Signal_InstallShutdownHandler(int signum);

// Requests a graceful shutdown, usable outside of a signal handler.
void Signal_RequestShutdown(int signum);

// TRUE once a shutdown has been requested by any source.
int Signal_ShutdownRequested(void);

// The signal that requested the shutdown, 0 if shutdown was not requested.
int Signal_ReceivedSignal(void);

#endif // WETMAN_UTILS_PROC_SIGNAL_H
