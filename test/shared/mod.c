#ifndef TEST_SHARED_MOD_C
#define TEST_SHARED_MOD_C

#include "endpoint/mod.c"
#include "persistence/mod.c"


void registerSharedTests(void)
{
    registerSharedPersistenceTests();
}

#endif // TEST_SHARED_MOD_C
