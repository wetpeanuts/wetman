#include <wetman/utils/net/endpoint_registry.h>
#include <wetman/utils/net/macro.h>
#include <wetman/utils/net/message.h>
#include <wetman/utils/test/macro.h>

#define TEST_ENDPOINT_REQUEST_SERIALIZED_LEN (sizeof(i32) + sizeof(i32))
#define TEST_ENDPOINT_RESPONSE_SERIALIZED_LEN (sizeof(i32) + sizeof(i32))

typedef struct {
    i32 dummy;
} TestEndpointRequest;

typedef struct {
    i32 dummy;
} TestEndpointResponse;


ReturnCode TestEndpoint(
        TestEndpointRequest* request,
        TestEndpointResponse* response)
{
    response->dummy = request->dummy;
    return RETURN_CODE_OK;
}

void TestEndpointRequestSerializer(TestEndpointRequest* req, DataStream* ds, Arena* arena)
{
    DataStream_PushI32(ds, req->dummy, arena);
}

void TestEndpointRequestDeserializer(TestEndpointRequest* req, DataStream* ds)
{
    req->dummy = DataStream_PopI32(ds);
}

void TestEndpointResponseSerializer(TestEndpointResponse* resp, DataStream* ds, Arena* arena)
{
    DataStream_PushI32(ds, resp->dummy, arena);
}

void TestEndpointResponseDeserializer(TestEndpointResponse* resp, DataStream* ds)
{
    resp->dummy = DataStream_PopI32(ds);
}

#define TEST_ENDPOINT_ID 0

ENDPOINT_IMPL_SERVER(TEST_ENDPOINT_ID, TestEndpoint)


TEST(EndpointRegistryTest_New)
{
    EndpointRegistry node = EndpointRegistry_New();
    EXPECT_EQ(node.__endpointCount, 0);
    for (size_t i = 0; i < ENDPOINT_REGISTRY_MAX_ENDPOINT_COUNT; ++i) {
        EXPECT_EQ(node.__endpoints[i].id, -1);
        EXPECT_EQ(node.__endpoints[i].handler, NULL);
        EXPECT_EQ(node.__endpoints[i].requestSerializer, NULL);
        EXPECT_EQ(node.__endpoints[i].requestDeserializer, NULL);
        EXPECT_EQ(node.__endpoints[i].requestFactory, NULL);
        EXPECT_EQ(node.__endpoints[i].responseSerializer, NULL);
        EXPECT_EQ(node.__endpoints[i].responseDeserializer, NULL);
        EXPECT_EQ(node.__endpoints[i].responseFactory, NULL);
    }
}

TEST(EndpointRegistryTest_RegisterEndpoint)
{
    EndpointRegistry node = EndpointRegistry_New();

    Endpoint endpoint = TestEndpoint_Create();
    EndpointRegistry_RegisterEndpoint(&node, endpoint);

    EXPECT_EQ(node.__endpointCount, 1);
    EXPECT_EQ(node.__endpoints[0].id, 0);
    EXPECT_NE(node.__endpoints[0].handler, NULL);
    EXPECT_NE(node.__endpoints[0].requestSerializer, NULL);
    EXPECT_NE(node.__endpoints[0].requestDeserializer, NULL);
    EXPECT_NE(node.__endpoints[0].requestFactory, NULL);
    EXPECT_NE(node.__endpoints[0].responseSerializer, NULL);
    EXPECT_NE(node.__endpoints[0].responseDeserializer, NULL);
    EXPECT_NE(node.__endpoints[0].responseFactory, NULL);

    // TODO: test register invalid endpoint
}

TEST(EndpointRegistryTest_CallEndpoint_Success)
{
    EndpointRegistry node = EndpointRegistry_New();

    Endpoint endpoint = TestEndpoint_Create();
    EndpointRegistry_RegisterEndpoint(&node, endpoint);

    ASSERT_EQ(node.__endpointCount, 1);
    ASSERT_EQ(node.__endpoints[0].id, 0);
    ASSERT_NE(node.__endpoints[0].handler, NULL);
    ASSERT_NE(node.__endpoints[0].requestSerializer, NULL);
    ASSERT_NE(node.__endpoints[0].requestDeserializer, NULL);
    ASSERT_NE(node.__endpoints[0].requestFactory, NULL);
    ASSERT_NE(node.__endpoints[0].responseSerializer, NULL);
    ASSERT_NE(node.__endpoints[0].responseDeserializer, NULL);
    ASSERT_NE(node.__endpoints[0].responseFactory, NULL);

    Arena arena = Arena_New();
    TestEndpointRequest request = {
        .dummy = 42,
    };
    DataStream requestData = DataStream_New();
    TestEndpointRequestSerializer(&request, &requestData, &arena);

    DataStream responseData = EndpointRegistry_CallEndpoint(&node, 0, &arena, &requestData);
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseData);

    ASSERT_EQ(responseHeader.returnCode, RETURN_CODE_OK);
    ASSERT_EQ(responseHeader.msgLen, TEST_ENDPOINT_RESPONSE_SERIALIZED_LEN);

    TestEndpointResponse response;
    TestEndpointResponseDeserializer(&response, &responseData);

    EXPECT_EQ(response.dummy, request.dummy);

    Arena_Free(&arena);
}

TEST(EndpointRegistryTest_CallEndpoint_InvalidEndpoint)
{
    EndpointRegistry endpointRegistry = EndpointRegistry_New();

    Arena arena = Arena_New();
    TestEndpointRequest request = {
        .dummy = 42,
    };
    DataStream requestData = DataStream_New();
    TestEndpointRequestSerializer(&request, &requestData, &arena);

    DataStream responseData = EndpointRegistry_CallEndpoint(&endpointRegistry, 0, &arena, &requestData);
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseData);

    ASSERT_EQ(responseHeader.returnCode, RETURN_CODE_ENDPOINT_NOT_INITIALIZED);
    ASSERT_EQ(responseHeader.msgLen, 0);

    responseData = EndpointRegistry_CallEndpoint(&endpointRegistry, ENDPOINT_REGISTRY_MAX_ENDPOINT_COUNT, &arena, &requestData);
    responseHeader = ResponseHeader_Deserialize(&responseData);

    ASSERT_EQ(responseHeader.returnCode, RETURN_CODE_INVALID_ENDPOINT_ID);
    ASSERT_EQ(responseHeader.msgLen, 0);

    Arena_Free(&arena);
}

TEST(EndpointRegistryTest_CallEndpoint_ValidEndpoint_InvalidRequest)
{
    EndpointRegistry node = EndpointRegistry_New();

    Endpoint endpoint = TestEndpoint_Create();
    EndpointRegistry_RegisterEndpoint(&node, endpoint);

    ASSERT_EQ(node.__endpointCount, 1);
    ASSERT_EQ(node.__endpoints[0].id, 0);
    ASSERT_NE(node.__endpoints[0].handler, NULL);
    ASSERT_NE(node.__endpoints[0].requestSerializer, NULL);
    ASSERT_NE(node.__endpoints[0].requestDeserializer, NULL);
    ASSERT_NE(node.__endpoints[0].requestFactory, NULL);
    ASSERT_NE(node.__endpoints[0].responseSerializer, NULL);
    ASSERT_NE(node.__endpoints[0].responseDeserializer, NULL);
    ASSERT_NE(node.__endpoints[0].responseFactory, NULL);

    Arena arena = Arena_New();
    DataStream requestData = DataStream_New();
    DataStream_PushU32(&requestData, 42, &arena);

    DataStream responseData = EndpointRegistry_CallEndpoint(&node, 0, &arena, &requestData);
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseData);

    EXPECT_EQ(responseHeader.returnCode, RETURN_CODE_FAILED_TO_PARSE_REQUEST);
    EXPECT_EQ(responseHeader.msgLen, 0);

    Arena_Free(&arena);
}
