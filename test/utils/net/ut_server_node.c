#include <wetman/utils/net/server_node.h>
#include <wetman/utils/test/macro.h>


typedef struct {
    int dummy;
} TestEndpointRequest;

typedef struct {
    int dummy;
} TestEndpointResponse;


ReturnCode TestEndpoint(
        void* request,
        void* response)
{
    ((TestEndpointResponse*)response)->dummy =
        ((TestEndpointRequest*)request)->dummy;
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

Endpoint TestEndpoint_Init(int endpointId)
{
    Endpoint endpoint = {
        .id = endpointId,
        .handler = TestEndpoint,
        .requestParser = TestEndpointRequestParser,
        .responseFactory = TestEndpointResponseFactory,
    };

    return endpoint;
}

TEST(ServerNodeTest_New)
{
    ServerNode node = ServerNode_New();
    EXPECT_EQ(node.endpointCount, 0);
    for (size_t i = 0; i < SERVER_NODE_MAX_ENDPOINT_COUNT; ++i) {
        EXPECT_EQ(node.endpoints[i].id, -1);
        EXPECT_EQ(node.endpoints[i].handler, NULL);
        EXPECT_EQ(node.endpoints[i].requestParser, NULL);
        EXPECT_EQ(node.endpoints[i].responseFactory, NULL);
    }
}

TEST(ServerNodeTest_RegisterEndpoint)
{
    ServerNode node = ServerNode_New();

    Endpoint endpoint = TestEndpoint_Init(0);
    ServerNode_RegisterEndpoint(&node, endpoint);

    EXPECT_EQ(node.endpointCount, 1);
    EXPECT_EQ(node.endpoints[0].id, 0);
    EXPECT_NE(node.endpoints[0].handler, NULL);
    EXPECT_NE(node.endpoints[0].requestParser, NULL);
    EXPECT_NE(node.endpoints[0].responseFactory, NULL);

    // TODO: test register invalid endpoint
}

TEST(ServerNodeTest_CallEndpoint)
{
    ServerNode node = ServerNode_New();

    Endpoint endpoint = TestEndpoint_Init(0);
    ServerNode_RegisterEndpoint(&node, endpoint);

    ASSERT_EQ(node.endpointCount, 1);
    ASSERT_EQ(node.endpoints[0].id, 0);
    ASSERT_NE(node.endpoints[0].handler, NULL);
    ASSERT_NE(node.endpoints[0].requestParser, NULL);
    ASSERT_NE(node.endpoints[0].responseFactory, NULL);

    Arena arena = Arena_New();
    Str data = Str_FromCStr("");
    int returnCode = ServerNode_CallEndpoint(&node, 0, &arena, data);

    EXPECT_EQ(returnCode, RETURN_CODE_OK);

    // TODO: test call invalid endpoint

    Arena_Free(&arena);
}
