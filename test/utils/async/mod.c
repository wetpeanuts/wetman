#include <wetman/utils/test/macro.h>

#include "ut_callback.c"
#include "ut_event_loop.c"


void registerUtilAsyncTests(void)
{
    REGISTER_TEST(CallbackTest_WithNoArgs);
    REGISTER_TEST(CallbackTest_WithArgs);

    REGISTER_TEST(EventLoopTest_Exec_NonRecursive);
    REGISTER_TEST(EventLoopTest_Exec_Recursive);
    REGISTER_TEST(EventLoopTest_Push_QueueOverflow);
}
