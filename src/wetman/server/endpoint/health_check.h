#ifndef WETMAN_SERVER_ENDPOINTS_HEALTH_CHECK_H
#define WETMAN_SERVER_ENDPOINTS_HEALTH_CHECK_H

#include <wetman/utils/data_stream.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/endpoint.h>
#include <wetman/utils/net/macro.h>


typedef struct {
    int dummy;
} Endpoint_HealthCheckRequest;

typedef struct {
    int dummy;
} Endpoint_HealthCheckResponse;

ENDPOINT_DECLARE(Endpoint_HealthCheck)

#endif // WETMAN_SERVER_ENDPOINTS_HEALTH_CHECK_H
