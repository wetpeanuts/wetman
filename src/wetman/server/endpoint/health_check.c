#include <wetman/server/endpoint/health_check.h>

#include <wetman/utils/net/macro.h>


ReturnCode Endpoint_HealthCheck(
        Endpoint_HealthCheckRequest* request,
        Endpoint_HealthCheckResponse* response)
{
    (void)request;
    (void)response;

    return RETURN_CODE_OK;
}

void Endpoint_HealthCheckRequestSerializer(
        Endpoint_HealthCheckRequest* request,
        DataStream*                  dataStream,
        Arena*                       arena)
{
    (void)request;
    (void)dataStream;
    (void)arena;
}

void Endpoint_HealthCheckRequestDeserializer(
        Endpoint_HealthCheckRequest* request,
        DataStream*                  dataStream)
{
    (void)request;
    (void)dataStream;
}

void Endpoint_HealthCheckResponseSerializer(
        Endpoint_HealthCheckResponse* response,
        DataStream*                   dataStream,
        Arena*                        arena)
{
    (void)response;
    (void)dataStream;
    (void)arena;
}

void Endpoint_HealthCheckResponseDeserializer(
        Endpoint_HealthCheckResponse* response,
        DataStream*                   dataStream)
{
    (void)response;
    (void)dataStream;
}

ENDPOINT_IMPL(0, Endpoint_HealthCheck)
