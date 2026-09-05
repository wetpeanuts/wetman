#include <wetman/utils/test/macro.h>

#include "ut_context.c"
#include "ut_lock.c"
#include "endpoint/mod.c"
#include "persistence/mod.c"

void registerServerTests(void)
{
    REGISTER_TEST(ServerContextTest_InitWorkspace_CreatesDirAndSymlink);
    REGISTER_TEST(ServerLockTest_Acquire_Success);
    REGISTER_TEST(ServerLockTest_Acquire_DuplicateFails);
    REGISTER_TEST(ServerLockTest_Release_CleansUpFile);
    REGISTER_TEST(ServerLockTest_AcquireAfterRelease_Success);
    registerServerEndpointTests();
    registerServerPersistenceTests();
}

