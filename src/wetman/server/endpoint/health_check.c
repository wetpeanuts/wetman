#include <wetman/server/endpoint/health_check.h>

#include <wetman/shared/endpoint/id.h>

#include <wetman/utils/net/macro.h>


ReturnCode Endpoint_HealthCheck(
        Endpoint_HealthCheckRequest* request,
        Endpoint_HealthCheckResponse* response)
{
    (void)request;
    (void)response;

    return RETURN_CODE_OK;
}

ENDPOINT_IMPL_SERVER(ENDPOINT_ID_HEALTH_CHECK, Endpoint_HealthCheck)
