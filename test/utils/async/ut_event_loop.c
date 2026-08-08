#include <wetman/utils/async/event_loop.h>
#include <wetman/utils/test/macro.h>


static int __EventLoopTest_globalNonRecursiveCounter = 0;
static int __EventLoopTest_globalRecursiveCounter = 0;

void __EventLoopTest_callbacNonRecursive(Callback* callbackMeta)
{
    (void)callbackMeta->arena;
    __EventLoopTest_globalNonRecursiveCounter++;
}

typedef struct __EventLoopTest_RecursivePayload {
    EventLoop* eventLoop;
} __EventLoopTest_RecursivePayload;

void __EventLoopTest_callbackRecursive(Callback* callbackMeta)
{
    (void)callbackMeta->arena;
    if (__EventLoopTest_globalRecursiveCounter == 1000) {
        return;
    };

    __EventLoopTest_globalRecursiveCounter++;

    __EventLoopTest_RecursivePayload* p = (__EventLoopTest_RecursivePayload*)callbackMeta->payload;

    Arena recursiveArena = Arena_New();
    __EventLoopTest_RecursivePayload* recursivePayload =
            (__EventLoopTest_RecursivePayload*)Arena_Alloc(
                    &recursiveArena, sizeof(__EventLoopTest_RecursivePayload));
    recursivePayload->eventLoop = p->eventLoop;

    Callback callback = Callback_New(
            __EventLoopTest_callbackRecursive,
            recursivePayload,
            recursiveArena);

    EventLoop_Push(p->eventLoop, callback);
}

TEST(EventLoopTest_Exec_NonRecursive)
{
    __EventLoopTest_globalNonRecursiveCounter = 0;

    EventLoop eventLoop = EventLoop_New();
    ASSERT_EQ(EventLoop_PendingCount(&eventLoop), 0);

    Callback callback1 = Callback_WithNoArgs(__EventLoopTest_callbacNonRecursive);
    Callback callback2 = Callback_WithNoArgs(__EventLoopTest_callbacNonRecursive);
    ASSERT(EventLoop_Push(&eventLoop, callback1));
    ASSERT(EventLoop_Push(&eventLoop, callback2));
    ASSERT_EQ(EventLoop_PendingCount(&eventLoop), 2);

    EventLoop_Exec(&eventLoop);
    ASSERT_EQ(EventLoop_PendingCount(&eventLoop), 0);

    EXPECT_EQ(__EventLoopTest_globalNonRecursiveCounter, 2);
}

TEST(EventLoopTest_Exec_Recursive)
{
    __EventLoopTest_globalNonRecursiveCounter = 0;

    EventLoop eventLoop = EventLoop_New();
    ASSERT_EQ(EventLoop_PendingCount(&eventLoop), 0);

    Arena arena = Arena_New();
    __EventLoopTest_RecursivePayload* payload =
            (__EventLoopTest_RecursivePayload*)Arena_Alloc(
                    &arena, sizeof(__EventLoopTest_RecursivePayload));
    payload->eventLoop = &eventLoop;

    Callback callback = Callback_New(
            __EventLoopTest_callbackRecursive,
            payload,
            arena);

    ASSERT(EventLoop_Push(&eventLoop, callback));
    ASSERT_EQ(EventLoop_PendingCount(&eventLoop), 1);

    EventLoop_Exec(&eventLoop);
    ASSERT_EQ(EventLoop_PendingCount(&eventLoop), 0);

    EXPECT_EQ(__EventLoopTest_globalRecursiveCounter, 1000);
}

TEST(EventLoopTest_Push_QueueOverflow)
{
    __EventLoopTest_globalNonRecursiveCounter = 0;

    EventLoop eventLoop = EventLoop_New();
    ASSERT_EQ(EventLoop_PendingCount(&eventLoop), 0);

    for (size_t i = 0; i <= 256; ++i) {
        Callback callback = Callback_WithNoArgs(__EventLoopTest_callbacNonRecursive);
        if (i < 256) {
            ASSERT(EventLoop_Push(&eventLoop, callback));
        } else {
            ASSERT(!EventLoop_Push(&eventLoop, callback));
        }
    }
    ASSERT_EQ(EventLoop_PendingCount(&eventLoop), 256);

    EventLoop_Exec(&eventLoop);
    ASSERT_EQ(EventLoop_PendingCount(&eventLoop), 0);

    EXPECT_EQ(__EventLoopTest_globalNonRecursiveCounter, 256);
}
