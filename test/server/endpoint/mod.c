#include <wetman/utils/test/macro.h>

#include "ut_health_check.c"
#include "ut_workspace_init.c"

void registerServerEndpointTests(void)
{
    REGISTER_TEST(HealthCheckTest_CallEndpoint_Success);
    REGISTER_TEST(WorkspaceInitTest_CallEndpoint_Success);
}
