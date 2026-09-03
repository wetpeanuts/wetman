#include <wetman/client/context.h>

#include <wetman/utils/net/client/unix_client.h>


#define SOCKET_PATH "/tmp/wetman_server.sock"


void ClientContext_Init(void)
{
    globalClientContext = (ClientContext) {
        .arena  = Arena_New(),
        .client = UnixClient_Connect(SOCKET_PATH),
    };
}

void ClientContext_Destroy(void)
{
    globalClientContext.client.disconnect(&globalClientContext.client);
    Arena_Free(&globalClientContext.arena);
}

ClientContext globalClientContext = {0};

