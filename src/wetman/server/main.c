#include <wetman/utils/net/endpoint_registry.h>
#include <wetman/utils/net/server.h>

#include <wetman/server/mod.c>


int main(void)
{
    EndpointRegistry endpointRegistry = EndpointRegistry_New();
    return Server_Run("/tmp/wetman_server.sock", &endpointRegistry);
}
