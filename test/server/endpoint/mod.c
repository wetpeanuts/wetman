#include <wetman/utils/test/macro.h>

#include "ut_health_check.c"
#include "ut_workspace_init.c"
#include "ut_workspace_delete.c"
#include "ut_workspace_list.c"
#include "ut_task_new.c"

void registerServerEndpointTests(void)
{
    REGISTER_TEST(HealthCheckTest_CallEndpoint_Success);
    REGISTER_TEST(WorkspaceInitTest_CallEndpoint_Success);
    REGISTER_TEST(WorkspaceDeleteTest_CallEndpoint_Success);
    REGISTER_TEST(WorkspaceDeleteTest_CallEndpoint_WorkspaceNotFound);
    REGISTER_TEST(WorkspaceListTest_CallEndpoint_Success);
    REGISTER_TEST(WorkspaceListTest_CallEndpoint_Empty);
    REGISTER_TEST(TaskNewTest_CallEndpoint_Success);
    REGISTER_TEST(TaskNewTest_CallEndpoint_WorkspaceNotFound);
}

