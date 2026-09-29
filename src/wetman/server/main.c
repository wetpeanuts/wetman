#include <wetman/utils/net/endpoint_registry.h>
#include <wetman/utils/net/server.h>
#include <wetman/utils/filesystem.h>

#include <wetman/server/context.h>
#include <wetman/server/lock.h>
#include <wetman/server/endpoint/health_check.h>
#include <wetman/server/endpoint/workspace_init.h>
#include <wetman/server/endpoint/workspace_delete.h>
#include <wetman/server/endpoint/workspace_list.h>
#include <wetman/server/endpoint/task_new.h>
#include <wetman/server/endpoint/task_get.h>
#include <wetman/server/endpoint/task_list.h>
#include <wetman/server/endpoint/task_delete.h>

#include <wetman/server/mod.c>

#include <wetman/utils/proc/signal.h>

#ifdef WETMAN_ENABLE_MEMORY_TRACKER
#include <wetman/utils/mem/memory_tracker.h>
#endif // WETMAN_ENABLE_MEMORY_TRACKER

int main(void)
{
#ifdef WETMAN_ENABLE_MEMORY_TRACKER
    MemoryTracker_Init(STDOUT_FILENO);

    // Ctrl+C sends SIGINT, Subprocess_Kill sends SIGTERM. Both request a
    // graceful shutdown so buffered events still get flushed.
    Signal_InstallShutdownHandler(SIGINT);
    Signal_InstallShutdownHandler(SIGTERM);
#endif // WETMAN_ENABLE_MEMORY_TRACKER

    Arena arena = Arena_New();
    char *wdirEnv = getenv("WETMAN_WDIR");
    Str wdir = wdirEnv
            ? Str_FromCStr(wdirEnv)
            : FS_PathJoin(
                    Str_FromCStr(getenv("HOME")),
                    Str_FromCStr(".wetman"),
                    &arena);

    FS_CreateDir(wdir);

    Str lockPath = FS_PathJoin(wdir, Str_FromCStr("server.lock"), &arena);
    if (!ServerLock_Acquire(lockPath)) {
        fprintf(stderr, "error: another server instance is already running\n");
        return 1;
    }

    ServerContext_Init(arena, wdir);

    EndpointRegistry endpointRegistry = EndpointRegistry_New();
    EndpointRegistry_RegisterEndpoint(&endpointRegistry,
            Endpoint_HealthCheck_Create());
    EndpointRegistry_RegisterEndpoint(&endpointRegistry,
            Endpoint_WorkspaceInit_Create());
    EndpointRegistry_RegisterEndpoint(&endpointRegistry,
            Endpoint_WorkspaceDelete_Create());
    EndpointRegistry_RegisterEndpoint(&endpointRegistry,
            Endpoint_WorkspaceList_Create());
    EndpointRegistry_RegisterEndpoint(&endpointRegistry,
            Endpoint_TaskNew_Create());
    EndpointRegistry_RegisterEndpoint(&endpointRegistry,
            Endpoint_TaskGet_Create());
    EndpointRegistry_RegisterEndpoint(&endpointRegistry,
            Endpoint_TaskList_Create());
    EndpointRegistry_RegisterEndpoint(&endpointRegistry,
            Endpoint_TaskDelete_Create());

    i32 result = Server_Run("/tmp/wetman_server.sock", &endpointRegistry);
    ServerContext_Destroy();
    ServerLock_Release();

#ifdef WETMAN_ENABLE_MEMORY_TRACKER
    MemoryTracker_FlushEvents();
#endif // WETMAN_ENABLE_MEMORY_TRACKER

    const int signal = Signal_ReceivedSignal();
    return (signal != 0) ? 128 + signal : result;
}

