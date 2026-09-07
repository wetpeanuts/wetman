#ifndef TEST_UTILS_NET_CLIENT_MOD_C
#define TEST_UTILS_NET_CLIENT_MOD_C

#include <wetman/utils/test/macro.h>

#include "ut_endpoint_client.c"
#include "ut_unix_client.c"

void registerUtilNetClientTests(void)
{
    REGISTER_TEST(EndpointClientTest_EchoStr);
    REGISTER_TEST(EndpointClientTest_EchoFd);
    REGISTER_TEST(UnixClientTest_EchoFd);
}

#endif // TEST_UTILS_NET_CLIENT_MOD_C
