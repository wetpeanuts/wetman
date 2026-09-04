#ifndef TEST_SHARED_ENDPOINT_ECHO_STR_H
#define TEST_SHARED_ENDPOINT_ECHO_STR_H

#include <wetman/utils/macro.h>
#include <wetman/utils/type.h>
#include <wetman/utils/net/macro.h>

typedef struct {
    Str value;
} TestEndpointEchoStr_Request;

typedef struct {
    Str value;
} TestEndpointEchoStr_Response;

static inline MAYBE_UNUSED void TestEndpointEchoStr_RequestSerializer(
        TestEndpointEchoStr_Request* req,
        DataStream*                  ds,
        Arena*                       arena)
{
    DataStream_PushStr(ds, req->value, arena);
}

static inline MAYBE_UNUSED void TestEndpointEchoStr_RequestDeserializer(
        TestEndpointEchoStr_Request* req,
        DataStream*                  ds)
{
    req->value = DataStream_PopStr(ds);
}

static inline MAYBE_UNUSED void TestEndpointEchoStr_ResponseSerializer(
        TestEndpointEchoStr_Response* resp,
        DataStream*                   ds,
        Arena*                        arena)
{
    DataStream_PushStr(ds, resp->value, arena);
}

static inline MAYBE_UNUSED void TestEndpointEchoStr_ResponseDeserializer(
        TestEndpointEchoStr_Response* resp,
        DataStream*                   ds,
        Arena*                        arena)
{
    resp->value = DataStream_PopStr(ds);
    (void)arena;
}

ENDPOINT_DECLARE_SERVER(TestEndpointEchoStr)

#endif // TEST_SHARED_ENDPOINT_ECHO_STR_H
