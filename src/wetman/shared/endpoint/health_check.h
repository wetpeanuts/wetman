#ifndef WETMAN_SHARED_ENDPOINT_HEALTH_CHECK_H
#define WETMAN_SHARED_ENDPOINT_HEALTH_CHECK_H

#include <wetman/utils/data_stream.h>
#include <wetman/utils/macro.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/endpoint.h>


typedef struct {
    int dummy;
} Endpoint_HealthCheckRequest;

typedef struct {
    int dummy;
} Endpoint_HealthCheckResponse;

static inline MAYBE_UNUSED void Endpoint_HealthCheckRequestSerializer(
        Endpoint_HealthCheckRequest* request,
        DataStream*                  dataStream,
        Arena*                       arena)
{
    (void)request;
    (void)dataStream;
    (void)arena;
}

static inline MAYBE_UNUSED void Endpoint_HealthCheckRequestDeserializer(
        Endpoint_HealthCheckRequest* request,
        DataStream*                  dataStream)
{
    (void)request;
    (void)dataStream;
}

static inline MAYBE_UNUSED void Endpoint_HealthCheckResponseSerializer(
        Endpoint_HealthCheckResponse* response,
        DataStream*                   dataStream,
        Arena*                        arena)
{
    (void)response;
    (void)dataStream;
    (void)arena;
}

static inline MAYBE_UNUSED void Endpoint_HealthCheckResponseDeserializer(
        Endpoint_HealthCheckResponse* response,
        DataStream*                   dataStream)
{
    (void)response;
    (void)dataStream;
}

#endif // WETMAN_SHARED_ENDPOINT_HEALTH_CHECK_H
