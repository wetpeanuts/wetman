#ifndef TEST_SHARED_ENDPOINT_DUMMY_ENDPOINT_H
#define TEST_SHARED_ENDPOINT_DUMMY_ENDPOINT_H

#include <wetman/utils/macro.h>
#include <wetman/utils/type.h>
#include <wetman/utils/net/macro.h>
#include <wetman/utils/net/message.h>

#define TEST_ENDPOINT_REQUEST_SERIALIZED_LEN (sizeof(i32) + sizeof(i32))
#define TEST_ENDPOINT_RESPONSE_SERIALIZED_LEN (sizeof(i32) + sizeof(i32))

typedef struct {
    i32 value;
} TestEndpointEchoI32_Request;

typedef struct {
    i32 value;
} TestEndpointEchoI32_Response;

static inline MAYBE_UNUSED void TestEndpointEchoI32_RequestSerializer(
        TestEndpointEchoI32_Request* req,
        Message*                     message,
        Arena*                       arena)
{
    DataStream_PushI32(&message->body, req->value, arena);
}

static inline MAYBE_UNUSED void TestEndpointEchoI32_RequestDeserializer(
        TestEndpointEchoI32_Request* req,
        Message*                     message,
        Arena*                       arena)
{
    req->value = DataStream_PopI32(&message->body);
    (void)arena;
}

static inline MAYBE_UNUSED void TestEndpointEchoI32_ResponseSerializer(
        TestEndpointEchoI32_Response* resp,
        Message*                      message,
        Arena*                        arena)
{
    DataStream_PushI32(&message->body, resp->value, arena);
}

static inline MAYBE_UNUSED void TestEndpointEchoI32_ResponseDeserializer(
        TestEndpointEchoI32_Response* resp,
        Message*                      message,
        Arena*                        arena)
{
    resp->value = DataStream_PopI32(&message->body);
    (void)arena;
}

ENDPOINT_DECLARE_SERVER(TestEndpointEchoI32)

#endif // TEST_SHARED_ENDPOINT_DUMMY_ENDPOINT_H
