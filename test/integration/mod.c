#include <wetman/utils/test/macro.h>

#include "it_healthcheck.c"

void registerIntegrationTests(void)
{
    REGISTER_TEST(IntegrationTest_HealthCheck_NoServerRunning);
    REGISTER_TEST(IntegrationTest_HealthCheck_Success);
}
