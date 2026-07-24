#include <wetman/utils/test/macro.h>

#include "ut_server_node.c"

void registerUtilNetTests(void)
{
    REGISTER_TEST(ServerNodeTest_New);
    REGISTER_TEST(ServerNodeTest_RegisterEndpoint);
    REGISTER_TEST(ServerNodeTest_CallEndpoint);
}
