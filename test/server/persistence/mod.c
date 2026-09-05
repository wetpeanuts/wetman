#ifndef TEST_SERVER_PERSISTENCE_MOD_C
#define TEST_SERVER_PERSISTENCE_MOD_C

#include <wetman/utils/test/macro.h>

#include "ut_task_state.c"


void registerServerPersistenceTests(void)
{
    REGISTER_TEST(TaskStateTest_WriteRead_Success);
    REGISTER_TEST(TaskStateTest_Write_FileDoesNotExist);
    REGISTER_TEST(TaskStateTest_Read_FileDoesNotExist);
    REGISTER_TEST(TaskStateTest_Read_WrongFormat);
}

#endif // TEST_SERVER_PERSISTENCE_MOD_C

