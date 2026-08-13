#ifndef TEST_SHARED_ENDPOINT_ECHO_STR_H
#define TEST_SHARED_ENDPOINT_ECHO_STR_H

#include <wetman/utils/macro.h>
#include <wetman/utils/type.h>
#include <wetman/utils/net/macro.h>

typedef struct {
    Str value;
} TestEndpointEchoStrRequest;

typedef struct {
    Str value;
} TestEndpointEchoStrResponse;

static inline MAYBE_UNUSED void TestEndpointEchoStrRequestSerializer(
        TestEndpointEchoStrRequest* req,
        DataStream*                 ds,
        Arena*                      arena)
{
    DataStream_PushStr(ds, req->value, arena);
}

static inline MAYBE_UNUSED void TestEndpointEchoStrRequestDeserializer(
        TestEndpointEchoStrRequest* req,
        DataStream*                 ds)
{
    req->value = DataStream_PopStr(ds);
}

static inline MAYBE_UNUSED void TestEndpointEchoStrResponseSerializer(
        TestEndpointEchoStrResponse* resp,
        DataStream*                  ds,
        Arena*                       arena)
{
    DataStream_PushStr(ds, resp->value, arena);
}

static inline MAYBE_UNUSED void TestEndpointEchoStrResponseDeserializer(
        TestEndpointEchoStrResponse* resp,
        DataStream*                  ds)
{
    resp->value = DataStream_PopStr(ds);
}

ENDPOINT_DECLARE_SERVER(TestEndpointEchoStr)

#endif // TEST_SHARED_ENDPOINT_ECHO_STR_H
