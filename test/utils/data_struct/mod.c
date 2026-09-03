#include <wetman/utils/test/macro.h>

#include "ut_str.c"
#include "ut_vec.c"
#include "ut_slice_i32.c"
#include "ut_slice_u32.c"
#include "ut_slice_u64.c"
#include "ut_slice_i64.c"
#include "ut_slice_str.c"

void registerUtilDataStructTests(void)
{
    REGISTER_TEST(StrTest_EqCStr);
    REGISTER_TEST(StrTest_EqStr);
    REGISTER_TEST(StrTest_FromCStr);
    REGISTER_TEST(StrTest_FromU64);
    REGISTER_TEST(StrTest_Concat);

    REGISTER_TEST(VecTest_New);
    REGISTER_TEST(VecTest_WithCapacity);
    REGISTER_TEST(VecTest_PushAt);
    REGISTER_TEST(VecTest_AtOutOfRange);

    REGISTER_TEST(SliceI32Test_CreateEmpty);
    REGISTER_TEST(SliceI32Test_FromData);
    REGISTER_TEST(SliceI32Test_FromDataWithPage);
    REGISTER_TEST(SliceI32Test_At);
    REGISTER_TEST(SliceI32Test_AtOutOfRange);
    REGISTER_TEST(SliceI32Test_ConcatBothEmpty);
    REGISTER_TEST(SliceI32Test_ConcatS2Empty);
    REGISTER_TEST(SliceI32Test_ConcatS1Empty);
    REGISTER_TEST(SliceI32Test_ConcatS2EmptyArenaBacked);
    REGISTER_TEST(SliceI32Test_ConcatS1EmptyArenaBacked);
    REGISTER_TEST(SliceI32Test_ConcatSequentialSamePage);
    REGISTER_TEST(SliceI32Test_ConcatSamePageNotSequential);
    REGISTER_TEST(SliceI32Test_ConcatDifferentPages);
    REGISTER_TEST(SliceI32Test_ConcatExternalInputs);

    REGISTER_TEST(SliceU32Test_CreateEmpty);
    REGISTER_TEST(SliceU32Test_FromData);
    REGISTER_TEST(SliceU32Test_FromDataWithPage);
    REGISTER_TEST(SliceU32Test_At);
    REGISTER_TEST(SliceU32Test_AtOutOfRange);
    REGISTER_TEST(SliceU32Test_ConcatBothEmpty);
    REGISTER_TEST(SliceU32Test_ConcatS2Empty);
    REGISTER_TEST(SliceU32Test_ConcatS1Empty);
    REGISTER_TEST(SliceU32Test_ConcatS2EmptyArenaBacked);
    REGISTER_TEST(SliceU32Test_ConcatS1EmptyArenaBacked);
    REGISTER_TEST(SliceU32Test_ConcatSequentialSamePage);
    REGISTER_TEST(SliceU32Test_ConcatSamePageNotSequential);
    REGISTER_TEST(SliceU32Test_ConcatDifferentPages);
    REGISTER_TEST(SliceU32Test_ConcatExternalInputs);

    REGISTER_TEST(SliceU64Test_CreateEmpty);
    REGISTER_TEST(SliceU64Test_FromData);
    REGISTER_TEST(SliceU64Test_FromDataWithPage);
    REGISTER_TEST(SliceU64Test_At);
    REGISTER_TEST(SliceU64Test_AtOutOfRange);
    REGISTER_TEST(SliceU64Test_ConcatBothEmpty);
    REGISTER_TEST(SliceU64Test_ConcatS2Empty);
    REGISTER_TEST(SliceU64Test_ConcatS1Empty);
    REGISTER_TEST(SliceU64Test_ConcatS2EmptyArenaBacked);
    REGISTER_TEST(SliceU64Test_ConcatS1EmptyArenaBacked);
    REGISTER_TEST(SliceU64Test_ConcatSequentialSamePage);
    REGISTER_TEST(SliceU64Test_ConcatSamePageNotSequential);
    REGISTER_TEST(SliceU64Test_ConcatDifferentPages);
    REGISTER_TEST(SliceU64Test_ConcatExternalInputs);

    REGISTER_TEST(SliceI64Test_CreateEmpty);
    REGISTER_TEST(SliceI64Test_FromData);
    REGISTER_TEST(SliceI64Test_FromDataWithPage);
    REGISTER_TEST(SliceI64Test_At);
    REGISTER_TEST(SliceI64Test_AtOutOfRange);
    REGISTER_TEST(SliceI64Test_ConcatBothEmpty);
    REGISTER_TEST(SliceI64Test_ConcatS2Empty);
    REGISTER_TEST(SliceI64Test_ConcatS1Empty);
    REGISTER_TEST(SliceI64Test_ConcatS2EmptyArenaBacked);
    REGISTER_TEST(SliceI64Test_ConcatS1EmptyArenaBacked);
    REGISTER_TEST(SliceI64Test_ConcatSequentialSamePage);
    REGISTER_TEST(SliceI64Test_ConcatSamePageNotSequential);
    REGISTER_TEST(SliceI64Test_ConcatDifferentPages);
    REGISTER_TEST(SliceI64Test_ConcatExternalInputs);

    REGISTER_TEST(SliceStrTest_CreateEmpty);
    REGISTER_TEST(SliceStrTest_FromData);
    REGISTER_TEST(SliceStrTest_FromDataWithPage);
    REGISTER_TEST(SliceStrTest_At);
    REGISTER_TEST(SliceStrTest_AtOutOfRange);
    REGISTER_TEST(SliceStrTest_ConcatBothEmpty);
    REGISTER_TEST(SliceStrTest_ConcatS2Empty);
    REGISTER_TEST(SliceStrTest_ConcatS1Empty);
    REGISTER_TEST(SliceStrTest_ConcatS2EmptyArenaBacked);
    REGISTER_TEST(SliceStrTest_ConcatS1EmptyArenaBacked);
    REGISTER_TEST(SliceStrTest_ConcatSequentialSamePage);
    REGISTER_TEST(SliceStrTest_ConcatDifferentPages);
    REGISTER_TEST(SliceStrTest_ConcatExternalInputs);
}
