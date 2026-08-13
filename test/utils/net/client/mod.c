#ifndef TEST_UTILS_NET_CLIENT_MOD_C
#define TEST_UTILS_NET_CLIENT_MOD_C

#include <wetman/utils/test/macro.h>

#include "ut_endpoint_client.c"

void registerUtilNetClientTests(void)
{
    REGISTER_TEST(EndpointClientTest_EchoStr);
}

#endif // TEST_UTILS_NET_CLIENT_MOD_C
