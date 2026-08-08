#include <wetman/utils/async/callback.h>
#include <wetman/utils/test/macro.h>


static int __callbackTestWithNoArgsGlobalCounter = 0;
static int __callbackTestWithArgsGlobalStatus = 0;

typedef struct __CallbackTestPayload {
    int newStatus;
} __CallbackTestPayload;

void callbackWithNoArgs(Callback* callbackMeta)
{
    (void)callbackMeta->arena;
    __callbackTestWithNoArgsGlobalCounter++;
}

void callbackWithArgs(Callback* callbackMeta)
{
    (void)callbackMeta->arena;
    __CallbackTestPayload* p = (__CallbackTestPayload*)callbackMeta->payload;
    __callbackTestWithArgsGlobalStatus = p->newStatus;
}


TEST(CallbackTest_WithNoArgs)
{
    Callback callback = Callback_WithNoArgs(callbackWithNoArgs);
    ASSERT(Callback_IsValid(&callback));

    Callback_Invoke(&callback);
    EXPECT(!Callback_IsValid(&callback));

    EXPECT_EQ(__callbackTestWithNoArgsGlobalCounter, 1);
}

TEST(CallbackTest_WithArgs)
{
    Arena arena = Arena_New();
    __CallbackTestPayload* payload = (__CallbackTestPayload*)Arena_Alloc(
            &arena, sizeof(__CallbackTestPayload));
    payload->newStatus = 42;

    Callback callback = Callback_New(
            callbackWithArgs,
            payload,
            arena);
    ASSERT(Callback_IsValid(&callback));

    Callback_Invoke(&callback);
    EXPECT(!Callback_IsValid(&callback));

    EXPECT_EQ(__callbackTestWithArgsGlobalStatus, 42);
}
