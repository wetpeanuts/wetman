#include <wetman/utils/test/macro.h>

#include "ut_endpoint_registry.c"

void registerUtilNetTests(void)
{
    REGISTER_TEST(EndpointRegistryTest_New);
    REGISTER_TEST(EndpointRegistryTest_RegisterEndpoint);
    REGISTER_TEST(EndpointRegistryTest_CallEndpoint);
}
