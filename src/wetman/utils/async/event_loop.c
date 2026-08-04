#include <wetman/utils/async/event_loop.h>

#include <wetman/utils/macro.h>


EventLoop EventLoop_New(void)
{
    EventLoop eventLoop = {
        .__size    = 0,
        .__nextPos = 0,
    };
    return eventLoop;
}

int EventLoop_Push(EventLoop* eventLoop, Callback callback)
{
    if (eventLoop->__size == __EVENT_LOOP_MAX_SIZE) {
        // TODO: log error
        return 0;
    }

    const size_t nextPos = (eventLoop->__nextPos + (eventLoop->__size++))
            & (__EVENT_LOOP_MAX_SIZE - 1);
    eventLoop->__callbacks[nextPos] = callback;

    return TRUE;
}

size_t EventLoop_PendingCount(const EventLoop* eventLoop)
{
    return eventLoop->__size;
}

void EventLoop_Exec(EventLoop* eventLoop)
{
    while (eventLoop->__size != 0) {
        Callback_Invoke(&eventLoop->__callbacks[eventLoop->__nextPos]);
        eventLoop->__nextPos = (eventLoop->__nextPos + 1) & (__EVENT_LOOP_MAX_SIZE - 1);
        eventLoop->__size--;
    }
}

