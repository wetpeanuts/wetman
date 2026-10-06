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
#include <wetman/utils/args/parser.h>
#include <wetman/utils/mem/memory_tracker.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#endif // WETMAN_ENABLE_MEMORY_TRACKER

int main(int argc, char** argv)
{
#ifdef WETMAN_ENABLE_MEMORY_TRACKER
    Args memOutArgs = {
        .len = 1,
    };
    memOutArgs.args[0] = (Arg) {
        .shortForm    = Str_FromCStr("-m"),
        .fullForm     = Str_FromCStr("--mem-out"),
        .required     = FALSE,
        .position     = ARG_POSITION_NONE,
        .value        = Str_CreateEmpty(),
        .initialized  = FALSE,
    };

    ArgsParser argsParser;
    ArgsParser_Init(&argsParser, &memOutArgs);
    ArgsParser_Parse(&argsParser, argc, argv, 1);
    if (argsParser.status != ARGS_PARSE_STATUS_OK) {
        ArgsParser_PrintError(stderr, &argsParser);
        return 1;
    }

    i32 fdMemOut = STDOUT_FILENO;
    if (memOutArgs.args[0].initialized) {
        Str memOutPath = memOutArgs.args[0].value;
        // Exclusive create: an existing file is an error, never overridden.
        fdMemOut = FS_OpenFile(memOutPath, O_WRONLY | O_CREAT | O_EXCL);
        if (fdMemOut < 0) {
            fprintf(stderr, "error: cannot create memory output file '%.*s': %s\n",
                    (i32)memOutPath.len, memOutPath.data, strerror(errno));
            return 1;
        }
    }

    MemoryTracker_Init(fdMemOut);

    // Ctrl+C sends SIGINT, Subprocess_Kill sends SIGTERM. Both request a
    // graceful shutdown so buffered events still get flushed.
    Signal_InstallShutdownHandler(SIGINT);
    Signal_InstallShutdownHandler(SIGTERM);
#else
    (void)argc;
    (void)argv;
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

