#include <wetman/utils/net/endpoint_registry.h>
#include <wetman/utils/net/server.h>

#include <wetman/server/endpoint/health_check.h>

#include <wetman/server/mod.c>


int main(void)
{
    EndpointRegistry endpointRegistry = EndpointRegistry_New();
    EndpointRegistry_RegisterEndpoint(
            &endpointRegistry,
            Endpoint_HealthCheck_Create());

    return Server_Run("/tmp/wetman_server.sock", &endpointRegistry);
}
