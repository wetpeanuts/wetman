#include <wetman/test/mod.c>
#include <wetman/test/utils/macro.h>


TEST(DummyTest)
{
    printf("Running some test\n");
}


int main(void)
{
    REGISTER_TEST(DummyTest);

    RUN_TESTS();
}
