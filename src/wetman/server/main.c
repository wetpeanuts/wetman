#include <wetman/utils/net/endpoint_registry.h>
#include <wetman/utils/net/server.h>

#include <wetman/server/context.h>
#include <wetman/server/endpoint/health_check.h>

#include <wetman/server/mod.c>


int main(void)
{
    Arena arena = Arena_New();
    Str wdir = FS_PathJoin(
            Str_FromCStr(getenv("HOME")),
            Str_FromCStr(".wetman"),
            &arena);

    ServerContext_Init(arena, wdir);

    EndpointRegistry endpointRegistry = EndpointRegistry_New();
    EndpointRegistry_RegisterEndpoint(
            &endpointRegistry,
            Endpoint_HealthCheck_Create());

    i32 result = Server_Run("/tmp/wetman_server.sock", &endpointRegistry);
    ServerContext_Destroy();

    return result;
}
