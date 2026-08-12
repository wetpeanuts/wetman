#include <wetman/utils/net/client.h>
#include <wetman/utils/net/macro.h>
#include <wetman/utils/net/message.h>
#include <wetman/utils/test/macro.h>


#define TEST_CLIENT_ENDPOINT_ID 7

typedef struct {
    i32 value;
} TestClientEndpointRequest;

typedef struct {
    i32 value;
} TestClientEndpointResponse;

static inline MAYBE_UNUSED void TestClientEndpointRequestSerializer(
        TestClientEndpointRequest* request,
        DataStream*                dataStream,
        Arena*                     arena)
{
    DataStream_PushI32(dataStream, request->value, arena);
}

static inline MAYBE_UNUSED void TestClientEndpointRequestDeserializer(
        TestClientEndpointRequest* request,
        DataStream*                dataStream)
{
    request->value = DataStream_PopI32(dataStream);
}

static inline MAYBE_UNUSED void TestClientEndpointResponseSerializer(
        TestClientEndpointResponse* response,
        DataStream*                 dataStream,
        Arena*                      arena)
{
    DataStream_PushI32(dataStream, response->value, arena);
}

static inline MAYBE_UNUSED void TestClientEndpointResponseDeserializer(
        TestClientEndpointResponse* response,
        DataStream*                 dataStream)
{
    response->value = DataStream_PopI32(dataStream);
}

ENDPOINT_IMPL_CLIENT(TEST_CLIENT_ENDPOINT_ID, TestClientEndpoint)


typedef struct {
    DataStream capturedRequest;
    DataStream response;
    int        failWithError;
} __FakeTransportContext;

static __FakeTransportContext __fakeTransportContext;

DataStream __FakeTransportRequest(
        Client*     client,
        DataStream* requestData,
        Arena*      arena)
{
    (void)client;
    (void)arena;

    __fakeTransportContext.capturedRequest = *requestData;

    if (__fakeTransportContext.failWithError) {
        __fakeTransportContext.response.lastResult = DATA_STREAM_RESULT_FAILED_READ;
    }

    return __fakeTransportContext.response;
}


TEST(ClientCallTest_Ok)
{
    Arena arena = Arena_New();

    TestClientEndpointResponse echoResponse = {
        .value = 999,
    };
    DataStream responseBody = DataStream_New();
    TestClientEndpointResponseSerializer(&echoResponse, &responseBody, &arena);

    ResponseHeader responseHeader = {
        .returnCode = RETURN_CODE_OK,
        .msgLen     = responseBody.__data.len,
    };
    DataStream responseStream = DataStream_New();
    ResponseHeader_Serialize(&responseHeader, &responseStream, &arena);
    DataStream_Append(&responseStream, &responseBody, &arena);

    __fakeTransportContext = (__FakeTransportContext) {
        .response = responseStream,
    };

    Client client = {
        .request = __FakeTransportRequest,
    };

    TestClientEndpointRequest request = {
        .value = 1234,
    };
    TestClientEndpointResponse response = {
        .value = 0,
    };

    ReturnCode returnCode = TestClientEndpoint_Call(&client, &request, &response);

    EXPECT_EQ(returnCode, RETURN_CODE_OK);
    EXPECT_EQ(response.value, 999);

    RequestHeader capturedHeader = RequestHeader_Deserialize(&__fakeTransportContext.capturedRequest);
    EXPECT_EQ(capturedHeader.endpointId, TEST_CLIENT_ENDPOINT_ID);

    TestClientEndpointRequest capturedRequestBody = { .value = 0 };
    TestClientEndpointRequestDeserializer(&capturedRequestBody, &__fakeTransportContext.capturedRequest);
    EXPECT_EQ(capturedRequestBody.value, request.value);

    DataStream expectedRequestBody = DataStream_New();
    TestClientEndpointRequestSerializer(&request, &expectedRequestBody, &arena);
    EXPECT_EQ(capturedHeader.msgLen, expectedRequestBody.__data.len);

    Arena_Free(&arena);
}

TEST(ClientCallTest_ErrorCode)
{
    Arena arena = Arena_New();

    ResponseHeader responseHeader = {
        .returnCode = RETURN_CODE_INVALID_ENDPOINT_ID,
        .msgLen     = 0,
    };
    DataStream responseStream = DataStream_New();
    ResponseHeader_Serialize(&responseHeader, &responseStream, &arena);

    __fakeTransportContext = (__FakeTransportContext) {
        .response = responseStream,
    };

    Client client = {
        .request = __FakeTransportRequest,
    };

    TestClientEndpointRequest request = {
        .value = 42,
    };
    TestClientEndpointResponse response = {
        .value = 0,
    };

    ReturnCode returnCode = TestClientEndpoint_Call(&client, &request, &response);

    EXPECT_EQ(returnCode, RETURN_CODE_INVALID_ENDPOINT_ID);
    EXPECT_EQ(response.value, 0);

    Arena_Free(&arena);
}

TEST(ClientCallTest_TransportError)
{
    Arena arena = Arena_New();

    __fakeTransportContext = (__FakeTransportContext) {
        .failWithError = TRUE,
    };

    Client client = {
        .request = __FakeTransportRequest,
    };

    TestClientEndpointRequest request = {
        .value = 42,
    };
    TestClientEndpointResponse response = {
        .value = 0,
    };

    ReturnCode returnCode = TestClientEndpoint_Call(&client, &request, &response);

    EXPECT_EQ(returnCode, RETURN_CODE_INTERNAL_ENDPOINT_ERROR);

    Arena_Free(&arena);
}

TEST(ClientCallTest_InvalidResponse)
{
    Arena arena = Arena_New();

    DataStream garbage = DataStream_New();
    DataStream_PushI32(&garbage, 123, &arena);

    __fakeTransportContext = (__FakeTransportContext) {
        .response = garbage,
    };

    Client client = {
        .request = __FakeTransportRequest,
    };

    TestClientEndpointRequest request = {
        .value = 42,
    };
    TestClientEndpointResponse response = {
        .value = 0,
    };

    ReturnCode returnCode = TestClientEndpoint_Call(&client, &request, &response);

    EXPECT_EQ(returnCode, RETURN_CODE_FAILED_TO_PARSE_REQUEST);

    Arena_Free(&arena);
}
