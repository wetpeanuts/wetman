#ifndef WETMAN_SHARED_ENDPOINT_HEALTH_CHECK_H
#define WETMAN_SHARED_ENDPOINT_HEALTH_CHECK_H

#include <wetman/utils/data_stream.h>
#include <wetman/utils/macro.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/endpoint.h>


// Empty struct is GNU extension
typedef struct {
    int __dummy;
} Endpoint_HealthCheck_Request;

typedef struct {
    int __dummy;
} Endpoint_HealthCheck_Response;

static inline MAYBE_UNUSED void Endpoint_HealthCheck_RequestSerializer(
        Endpoint_HealthCheck_Request* req,
        DataStream*                   ds,
        Arena*                        arena)
{
    (void)req;
    (void)ds;
    (void)arena;
}

static inline MAYBE_UNUSED void Endpoint_HealthCheck_RequestDeserializer(
        Endpoint_HealthCheck_Request* req,
        DataStream*                   ds)
{
    (void)req;
    (void)ds;
}

static inline MAYBE_UNUSED void Endpoint_HealthCheck_ResponseSerializer(
        Endpoint_HealthCheck_Response* resp,
        DataStream*                    ds,
        Arena*                         arena)
{
    (void)resp;
    (void)ds;
    (void)arena;
}

static inline MAYBE_UNUSED void Endpoint_HealthCheck_ResponseDeserializer(
        Endpoint_HealthCheck_Response* resp,
        DataStream*                    ds)
{
    (void)resp;
    (void)ds;
}

#endif // WETMAN_SHARED_ENDPOINT_HEALTH_CHECK_H
