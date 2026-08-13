#ifndef TEST_SHARED_ENDPOINT_DUMMY_ENDPOINT_H
#define TEST_SHARED_ENDPOINT_DUMMY_ENDPOINT_H

#include <wetman/utils/macro.h>
#include <wetman/utils/type.h>
#include <wetman/utils/net/macro.h>

#define TEST_ENDPOINT_REQUEST_SERIALIZED_LEN (sizeof(i32) + sizeof(i32))
#define TEST_ENDPOINT_RESPONSE_SERIALIZED_LEN (sizeof(i32) + sizeof(i32))

typedef struct {
    i32 value;
} TestEndpointEchoI32Request;

typedef struct {
    i32 value;
} TestEndpointEchoI32Response;

static inline MAYBE_UNUSED void TestEndpointEchoI32RequestSerializer(
        TestEndpointEchoI32Request* req,
        DataStream*                 ds,
        Arena*                      arena)
{
    DataStream_PushI32(ds, req->value, arena);
}

static inline MAYBE_UNUSED void TestEndpointEchoI32RequestDeserializer(
        TestEndpointEchoI32Request* req,
        DataStream*                 ds)
{
    req->value = DataStream_PopI32(ds);
}

static inline MAYBE_UNUSED void TestEndpointEchoI32ResponseSerializer(
        TestEndpointEchoI32Response* resp,
        DataStream*                  ds,
        Arena*                       arena)
{
    DataStream_PushI32(ds, resp->value, arena);
}

static inline MAYBE_UNUSED void TestEndpointEchoI32ResponseDeserializer(
        TestEndpointEchoI32Response* resp,
        DataStream*                  ds)
{
    resp->value = DataStream_PopI32(ds);
}

ENDPOINT_DECLARE_SERVER(TestEndpointEchoI32)

#endif // TEST_SHARED_ENDPOINT_DUMMY_ENDPOINT_H
