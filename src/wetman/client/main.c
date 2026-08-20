#include <stdio.h>

#include <wetman/client/endpoint/health_check.h>
#include <wetman/utils/net/client/unix_client.h>

#include <wetman/client/mod.c>


#define SOCKET_PATH "/tmp/wetman_server.sock"

int main(void)
{
    Client client = UnixClient_Connect(SOCKET_PATH);

    Endpoint_HealthCheck_Request request = { 0 };
    Endpoint_HealthCheck_Response response = { 0 };

    ReturnCode returnCode = Endpoint_HealthCheck_Call(&client, &request, &response);

    printf("Health check returned status code: %d\n", returnCode);

    client.disconnect(&client);

    return (int)returnCode;
}
