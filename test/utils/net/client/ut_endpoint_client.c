#include <wetman/utils/net/client/endpoint_client.h>
#include <wetman/utils/net/endpoint_registry.h>
#include <wetman/utils/net/macro.h>
#include <wetman/utils/test/macro.h>

#include "../../../shared/endpoint/id.h"
#include "../../../shared/endpoint/echo_str.h"

ENDPOINT_IMPL_CLIENT(TEST_ENDPOINT_ID_ECHO_STR, TestEndpointEchoStr)


TEST(EndpointClientTest_EchoStr)
{
    EndpointRegistry endpointRegistry = EndpointRegistry_New();
    Endpoint endpoint = TestEndpointEchoStr_Create();
    EndpointRegistry_RegisterEndpoint(&endpointRegistry, endpoint);
    ASSERT_EQ(endpointRegistry.__endpointCount, 1);

    Client client = EndpointClient_Connect(&endpointRegistry);

    TestEndpointEchoStr_Request request = {
        .value = Str_FromCStr("hello, endpoint client!"),
    };
    TestEndpointEchoStr_Response response = {
        .value = Str_FromCStr(""),
    };

    Arena arena = Arena_New();

    ReturnCode returnCode = TestEndpointEchoStr_Call(&client, &request, &response, &arena);

    EXPECT_EQ(returnCode, RETURN_CODE_OK);
    EXPECT(Str_EqStr(response.value, request.value));

    Arena_Free(&arena);
    client.disconnect(&client);
}
