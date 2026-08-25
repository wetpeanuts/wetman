#include <wetman/utils/net/endpoint_registry.h>
#include <wetman/utils/net/server.h>
#include <wetman/utils/filesystem.h>

#include <wetman/server/context.h>
#include <wetman/server/lock.h>
#include <wetman/server/endpoint/health_check.h>
#include <wetman/server/endpoint/workspace_init.h>
#include <wetman/server/endpoint/workspace_delete.h>

#include <wetman/server/mod.c>


int main(void)
{
    Arena arena = Arena_New();
    Str wdir = FS_PathJoin(
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

    i32 result = Server_Run("/tmp/wetman_server.sock", &endpointRegistry);
    ServerContext_Destroy();
    ServerLock_Release();

    return result;
}
