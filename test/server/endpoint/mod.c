#include <wetman/utils/test/macro.h>

#include "ut_health_check.c"

void registerServerEndpointTests(void)
{
    REGISTER_TEST(HealthCheckTest_CallEndpoint_Success);
}
