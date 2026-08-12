#include <stdio.h>

#include <wetman/client/endpoint/health_check.h>
#include <wetman/utils/net/client.h>

#include <wetman/client/mod.c>


#define SOCKET_PATH "/tmp/wetman_server.sock"

int main(void)
{
    Client client = Client_New(SOCKET_PATH);

    Endpoint_HealthCheckRequest request = { 0 };
    Endpoint_HealthCheckResponse response = { 0 };

    ReturnCode returnCode = Endpoint_HealthCheck_Call(&client, &request, &response);

    printf("Health check returned status code: %d\n", returnCode);

    return (int)returnCode;
}
