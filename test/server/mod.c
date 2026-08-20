#include <wetman/utils/test/macro.h>

#include "ut_context.c"
#include "endpoint/mod.c"

void registerServerTests(void)
{
    REGISTER_TEST(ServerContextTest_InitWorkspace_CreatesDirAndSymlink);
    registerServerEndpointTests();
}
