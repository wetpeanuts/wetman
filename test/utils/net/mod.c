#include <wetman/utils/test/macro.h>

#include "ut_endpoint_registry.c"
#include "ut_message.c"

void registerUtilNetTests(void)
{
    REGISTER_TEST(EndpointRegistryTest_New);
    REGISTER_TEST(EndpointRegistryTest_RegisterEndpoint);
    REGISTER_TEST(EndpointRegistryTest_CallEndpoint);

    REGISTER_TEST(MessageTest_RequestHeader_SerializeDeserialize);
    REGISTER_TEST(MessageTest_ResponseHeader_SerializeDeserialize);
}
