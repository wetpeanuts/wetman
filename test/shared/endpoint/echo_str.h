#ifndef TEST_SHARED_ENDPOINT_ECHO_STR_H
#define TEST_SHARED_ENDPOINT_ECHO_STR_H

#include <wetman/utils/macro.h>
#include <wetman/utils/type.h>
#include <wetman/utils/net/macro.h>
#include <wetman/utils/net/message.h>

typedef struct {
    Str value;
} TestEndpointEchoStr_Request;

typedef struct {
    Str value;
} TestEndpointEchoStr_Response;

static inline MAYBE_UNUSED void TestEndpointEchoStr_RequestSerializer(
        TestEndpointEchoStr_Request* req,
        Message*                     message,
        Arena*                       arena)
{
    DataStream_PushStr(&message->bodyStream, req->value, arena);
}

static inline MAYBE_UNUSED void TestEndpointEchoStr_RequestDeserializer(
        TestEndpointEchoStr_Request* req,
        Message*                     message,
        Arena*                       arena)
{
    req->value = DataStream_PopStr(&message->bodyStream);
    (void)arena;
}

static inline MAYBE_UNUSED void TestEndpointEchoStr_ResponseSerializer(
        TestEndpointEchoStr_Response* resp,
        Message*                      message,
        Arena*                        arena)
{
    DataStream_PushStr(&message->bodyStream, resp->value, arena);
}

static inline MAYBE_UNUSED void TestEndpointEchoStr_ResponseDeserializer(
        TestEndpointEchoStr_Response* resp,
        Message*                      message,
        Arena*                        arena)
{
    resp->value = DataStream_PopStr(&message->bodyStream);
    (void)arena;
}

ENDPOINT_DECLARE_SERVER(TestEndpointEchoStr)

#endif // TEST_SHARED_ENDPOINT_ECHO_STR_H
