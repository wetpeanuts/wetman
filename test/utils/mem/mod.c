#include <wetman/utils/test/macro.h>

#include "ut_arena.c"
#include "ut_memory_tracker_reader.c"

void registerUtilMemTests(void)
{
    REGISTER_TEST(ArenaTest_NewFree);
    REGISTER_TEST(ArenaTest_WithPageCapacity);
    REGISTER_TEST(ArenaTest_NewReset);
    REGISTER_TEST(ArenaTest_Alloc);
    REGISTER_TEST(ArenaTest_AllocWithPage);
    REGISTER_TEST(ArenaTest_AllocWithPageLarge);
    REGISTER_TEST(ArenaTest_CanAllocOnSamePage);

    REGISTER_TEST(MemoryTrackerReaderTest_ReadValidDump);
    REGISTER_TEST(MemoryTrackerReaderTest_ReadErrors);
    REGISTER_TEST(MemoryTrackerReaderTest_VerifyConfirmsCleanDump);
    REGISTER_TEST(MemoryTrackerReaderTest_VerifyDetectsLeak);
    REGISTER_TEST(MemoryTrackerReaderTest_VerifyDetectsInvalidTransitions);
}
