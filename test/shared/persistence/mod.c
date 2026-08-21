#ifndef TEST_SHARED_PERSISTENCE_MOD_C
#define TEST_SHARED_PERSISTENCE_MOD_C

#include <wetman/utils/test/macro.h>

#include "ut_workspace_config.c"


void registerSharedPersistenceTests(void)
{
    REGISTER_TEST(WorkspaceConfigTest_WriteRead_Success);
    REGISTER_TEST(WorkspaceConfigTest_Write_FileDoesNotExist);
    REGISTER_TEST(WorkspaceConfigTest_Read_FileDoesNotExist);
    REGISTER_TEST(WorkspaceConfigTest_Read_WrongFormat);
}

#endif // TEST_SHARED_PERSISTENCE_MOD_C
