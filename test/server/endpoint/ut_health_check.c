#include <wetman/server/endpoint/health_check.h>
#include <wetman/shared/endpoint/id.h>
#include <wetman/utils/net/endpoint_registry.h>
#include <wetman/utils/net/message.h>
#include <wetman/utils/test/macro.h>


TEST(HealthCheckTest_CallEndpoint_Success)
{
    EndpointRegistry endpointRegistry = EndpointRegistry_New();

    Endpoint endpoint = Endpoint_HealthCheck_Create();
    EndpointRegistry_RegisterEndpoint(&endpointRegistry, endpoint);

    ASSERT_EQ(endpointRegistry.__endpointCount, 1);
    ASSERT_EQ(endpointRegistry.__endpoints[ENDPOINT_ID_HEALTH_CHECK].id, ENDPOINT_ID_HEALTH_CHECK);

    Arena arena = Arena_New();
    Message requestMessage = Message_New();

    Message responseMessage = EndpointRegistry_CallEndpoint(
            &endpointRegistry,
            ENDPOINT_ID_HEALTH_CHECK,
            &arena,
            &requestMessage);
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseMessage.header);

    EXPECT_EQ(responseHeader.returnCode, RETURN_CODE_OK);
    EXPECT_EQ(responseHeader.msgLen, 0);

    Arena_Free(&arena);
}
