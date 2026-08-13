#include <wetman/utils/net/endpoint_registry.h>
#include <wetman/utils/net/macro.h>
#include <wetman/utils/net/message.h>
#include <wetman/utils/test/macro.h>

#include "../../shared/endpoint/id.h"
#include "../../shared/endpoint/echo_i32.h"


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

    Endpoint endpoint = TestEndpointEchoI32_Create();
    EndpointRegistry_RegisterEndpoint(&node, endpoint);

    EXPECT_EQ(node.__endpointCount, 1);
    EXPECT_EQ(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].id, TEST_ENDPOINT_ID_ECHO_I32);
    EXPECT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].handler, NULL);
    EXPECT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].requestSerializer, NULL);
    EXPECT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].requestDeserializer, NULL);
    EXPECT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].requestFactory, NULL);
    EXPECT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].responseSerializer, NULL);
    EXPECT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].responseDeserializer, NULL);
    EXPECT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].responseFactory, NULL);

    // TODO: test register invalid endpoint
}

TEST(EndpointRegistryTest_CallEndpoint_Success)
{
    EndpointRegistry node = EndpointRegistry_New();

    Endpoint endpoint = TestEndpointEchoI32_Create();
    EndpointRegistry_RegisterEndpoint(&node, endpoint);

    ASSERT_EQ(node.__endpointCount, 1);
    ASSERT_EQ(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].id, TEST_ENDPOINT_ID_ECHO_I32);
    ASSERT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].handler, NULL);
    ASSERT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].requestSerializer, NULL);
    ASSERT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].requestDeserializer, NULL);
    ASSERT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].requestFactory, NULL);
    ASSERT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].responseSerializer, NULL);
    ASSERT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].responseDeserializer, NULL);
    ASSERT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].responseFactory, NULL);

    Arena arena = Arena_New();
    TestEndpointEchoI32Request request = {
        .value = 42,
    };
    DataStream requestData = DataStream_New();
    TestEndpointEchoI32RequestSerializer(&request, &requestData, &arena);

    DataStream responseData = EndpointRegistry_CallEndpoint(
            &node, TEST_ENDPOINT_ID_ECHO_I32, &arena, &requestData);
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseData);

    ASSERT_EQ(responseHeader.returnCode, RETURN_CODE_OK);
    ASSERT_EQ(responseHeader.msgLen, TEST_ENDPOINT_RESPONSE_SERIALIZED_LEN);

    TestEndpointEchoI32Response response;
    TestEndpointEchoI32ResponseDeserializer(&response, &responseData);

    EXPECT_EQ(response.value, request.value);

    Arena_Free(&arena);
}

TEST(EndpointRegistryTest_CallEndpoint_InvalidEndpoint)
{
    EndpointRegistry endpointRegistry = EndpointRegistry_New();

    Arena arena = Arena_New();
    TestEndpointEchoI32Request request = {
        .value = 42,
    };
    DataStream requestData = DataStream_New();
    TestEndpointEchoI32RequestSerializer(&request, &requestData, &arena);

    DataStream responseData = EndpointRegistry_CallEndpoint(
            &endpointRegistry, TEST_ENDPOINT_ID_ECHO_I32, &arena, &requestData);
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

    Endpoint endpoint = TestEndpointEchoI32_Create();
    EndpointRegistry_RegisterEndpoint(&node, endpoint);

    ASSERT_EQ(node.__endpointCount, 1);
    ASSERT_EQ(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].id, TEST_ENDPOINT_ID_ECHO_I32);
    ASSERT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].handler, NULL);
    ASSERT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].requestSerializer, NULL);
    ASSERT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].requestDeserializer, NULL);
    ASSERT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].requestFactory, NULL);
    ASSERT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].responseSerializer, NULL);
    ASSERT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].responseDeserializer, NULL);
    ASSERT_NE(node.__endpoints[TEST_ENDPOINT_ID_ECHO_I32].responseFactory, NULL);

    Arena arena = Arena_New();
    DataStream requestData = DataStream_New();
    DataStream_PushU32(&requestData, 42, &arena);

    DataStream responseData = EndpointRegistry_CallEndpoint(
            &node, TEST_ENDPOINT_ID_ECHO_I32, &arena, &requestData);
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseData);

    EXPECT_EQ(responseHeader.returnCode, RETURN_CODE_FAILED_TO_PARSE_REQUEST);
    EXPECT_EQ(responseHeader.msgLen, 0);

    Arena_Free(&arena);
}
