#include <wetman/utils/net/endpoint_registry.h>
#include <wetman/utils/net/macro.h>
#include <wetman/utils/test/macro.h>


typedef struct {
    int dummy;
} TestEndpointRequest;

typedef struct {
    int dummy;
} TestEndpointResponse;


ReturnCode TestEndpoint(
        TestEndpointRequest* request,
        TestEndpointResponse* response)
{
    response->dummy = request->dummy;
    printf("Set dummy value: %d\n", ((TestEndpointResponse*)response)->dummy);
    return RETURN_CODE_OK;
}

void* TestEndpointRequestParser(Arena* arena, Str data MAYBE_UNUSED)
{
    TestEndpointRequest* request = (TestEndpointRequest*)Arena_Alloc(
            arena, sizeof(TestEndpointRequest));
    request->dummy = 42;
    return (void*)request;
}

void* TestEndpointResponseFactory(Arena* arena)
{
    TestEndpointResponse* response = (TestEndpointResponse*)Arena_Alloc(
            arena, sizeof(TestEndpointResponse));
    return (void*)response;
}

#define TEST_ENDPOINT_ID 0

DELCARE_ENDPOINT(TEST_ENDPOINT_ID, TestEndpoint)


TEST(EndpointRegistryTest_New)
{
    EndpointRegistry node = EndpointRegistry_New();
    EXPECT_EQ(node.__endpointCount, 0);
    for (size_t i = 0; i < ENDPOINT_REGISTRY_MAX_ENDPOINT_COUNT; ++i) {
        EXPECT_EQ(node.__endpoints[i].id, -1);
        EXPECT_EQ(node.__endpoints[i].handler, NULL);
        EXPECT_EQ(node.__endpoints[i].requestParser, NULL);
        EXPECT_EQ(node.__endpoints[i].responseFactory, NULL);
    }
}

TEST(EndpointRegistryTest_RegisterEndpoint)
{
    EndpointRegistry node = EndpointRegistry_New();

    Endpoint endpoint = TestEndpoint_Init();
    EndpointRegistry_RegisterEndpoint(&node, endpoint);

    EXPECT_EQ(node.__endpointCount, 1);
    EXPECT_EQ(node.__endpoints[0].id, 0);
    EXPECT_NE(node.__endpoints[0].handler, NULL);
    EXPECT_NE(node.__endpoints[0].requestParser, NULL);
    EXPECT_NE(node.__endpoints[0].responseFactory, NULL);

    // TODO: test register invalid endpoint
}

TEST(EndpointRegistryTest_CallEndpoint)
{
    EndpointRegistry node = EndpointRegistry_New();

    Endpoint endpoint = TestEndpoint_Init();
    EndpointRegistry_RegisterEndpoint(&node, endpoint);

    ASSERT_EQ(node.__endpointCount, 1);
    ASSERT_EQ(node.__endpoints[0].id, 0);
    ASSERT_NE(node.__endpoints[0].handler, NULL);
    ASSERT_NE(node.__endpoints[0].requestParser, NULL);
    ASSERT_NE(node.__endpoints[0].responseFactory, NULL);

    Arena arena = Arena_New();
    Str data = Str_FromCStr("");
    int returnCode = EndpointRegistry_CallEndpoint(&node, 0, &arena, data);

    EXPECT_EQ(returnCode, RETURN_CODE_OK);

    // TODO: test call invalid endpoint

    Arena_Free(&arena);
}
