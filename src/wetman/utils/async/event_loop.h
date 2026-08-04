#ifndef WETMAN_UTILS_ASYNC_EVENT_LOOP_H
#define WETMAN_UTILS_ASYNC_EVENT_LOOP_H

#include <wetman/utils/async/callback.h>


#define __EVENT_LOOP_MAX_SIZE 256

typedef struct EventLoop {
    Callback __callbacks[__EVENT_LOOP_MAX_SIZE];
    size_t   __size;
    size_t   __nextPos; // Callback to be executed next
} EventLoop;


EventLoop EventLoop_New(void);
int EventLoop_Push(EventLoop* eventLoop, Callback callback);
size_t EventLoop_PendingCount(const EventLoop* eventLoop);
void EventLoop_Exec(EventLoop* eventLoop);

#endif // WETMAN_UTILS_ASYNC_EVENT_LOOP_H 
