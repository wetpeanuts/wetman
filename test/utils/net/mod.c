#include <wetman/utils/test/macro.h>

#include "client/mod.c"

#include "ut_endpoint_registry.c"
#include "ut_message.c"

void registerUtilNetTests(void)
{
    registerUtilNetClientTests();

    REGISTER_TEST(EndpointRegistryTest_New);
    REGISTER_TEST(EndpointRegistryTest_RegisterEndpoint);
    REGISTER_TEST(EndpointRegistryTest_CallEndpoint_Success);
    REGISTER_TEST(EndpointRegistryTest_CallEndpoint_InvalidEndpoint);
    REGISTER_TEST(EndpointRegistryTest_CallEndpoint_ValidEndpoint_InvalidRequest);

    REGISTER_TEST(MessageTest_RequestHeader_SerializeDeserialize);
    REGISTER_TEST(MessageTest_ResponseHeader_SerializeDeserialize);
}
