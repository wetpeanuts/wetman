#include <wetman/utils/mem/memory_tracker.h>

#ifdef WETMAN_ENABLE_MEMORY_TRACKER

#include <wetman/utils/mem/arena.h>
#include <wetman/utils/time.h>

#include <stdio.h>
#include <stdlib.h>


// Tracker stays inert until MemoryTracker_Init provides a valid fdOutput,
// so binaries that never initialize it do not record anything.
static int __MemoryTracker_Enabled(void)
{
    return __globalMemoryTracker.fdOutput >= 0;
}

// Reserves the next slot, stamping it with the record time.
static MemoryEvent* __MemoryTracker_PushEvent(MemoryEventType type)
{
    MemoryEvent* event = &__globalMemoryTracker.events[__globalMemoryTracker.eventCount++];
    event->type        = type;
    event->timestamp = currentUnixTimestampMs();
    return event;
}


void __MemoryEvent_Flush(MemoryEvent* event, i32 fd)
{
    switch (event->type) {
        case MEMORY_EVENT_UNDEFINED:
            // Ignore undefined event
            break;
        case MEMORY_EVENT_ARENA_CREATED:
            dprintf(fd, "MemoryEvent { type: ARENA_CREATED, timestamp: %lld, arenaId: %zu }\n",
                    (long long)event->timestamp,
                    event->event.arenaCreated.arenaId);
            break;
        case MEMORY_EVENT_ARENA_FREED:
            dprintf(fd, "MemoryEvent { type: ARENA_FREED, timestamp: %lld, arenaId: %zu }\n",
                    (long long)event->timestamp,
                    event->event.arenaFreed.arenaId);
            break;
        case MEMORY_EVENT_ARENA_RESET:
            dprintf(fd, "MemoryEvent { type: ARENA_RESET, timestamp: %lld, arenaId: %zu }\n",
                    (long long)event->timestamp,
                    event->event.arenaReset.arenaId);
            break;
        case MEMORY_EVENT_MEMORY_ALLOCATED:
            dprintf(fd, "MemoryEvent { type: MEMORY_ALLOCATED, timestamp: %lld, arenaId: %zu, allocSize: %zu }\n",
                    (long long)event->timestamp,
                    event->event.memoryAllocated.arenaId,
                    event->event.memoryAllocated.allocSize);
            break;
    }
}

void __MemoryTracker_TryFlushEvents(void)
{
    if (__globalMemoryTracker.eventCount < MEMORY_TRACKER_MAX_EVENT_COUNT) {
        return;
    }

    MemoryTracker_FlushEvents();
}


void MemoryTracker_Init(i32 fdOutput)
{
    __globalMemoryTracker = (MemoryTracker) {
        .eventCount  = 0,
        .fdOutput    = fdOutput,
        .nextArenaId = 0,
    };

    for (usize i = 0; i < MEMORY_TRACKER_MAX_EVENT_COUNT ; ++i) {
        __globalMemoryTracker.events[i].type = MEMORY_EVENT_UNDEFINED;
        __globalMemoryTracker.events[i].event.undefined = (MemoryEvent_Undefined) {0};
    }

    // Safety net for the exit() paths that bypass the regular shutdown,
    // e.g. socket setup failures in __Server_CreateSocket.
    atexit(MemoryTracker_FlushEvents);
}

void MemoryTracker_ArenaCreated(Arena* arena)
{
    if (!__MemoryTracker_Enabled()) return;

    arena->__id = __globalMemoryTracker.nextArenaId++;

    MemoryEvent* event = __MemoryTracker_PushEvent(MEMORY_EVENT_ARENA_CREATED);
    event->event.arenaCreated = (MemoryEvent_ArenaCreated) {
        .arenaId = arena->__id,
    };

    __MemoryTracker_TryFlushEvents();
}

void MemoryTracker_ArenaFreed(ArenaId arenaId)
{
    if (!__MemoryTracker_Enabled()) return;

    MemoryEvent* event = __MemoryTracker_PushEvent(MEMORY_EVENT_ARENA_FREED);
    event->event.arenaFreed = (MemoryEvent_ArenaFreed) {
        .arenaId = arenaId,
    };

    __MemoryTracker_TryFlushEvents();
}

void MemoryTracker_ArenaReset(ArenaId arenaId)
{
    if (!__MemoryTracker_Enabled()) return;

    MemoryEvent* event = __MemoryTracker_PushEvent(MEMORY_EVENT_ARENA_RESET);
    event->event.arenaReset = (MemoryEvent_ArenaReset) {
        .arenaId = arenaId,
    };

    __MemoryTracker_TryFlushEvents();
}

void MemoryTracker_MemoryAllocated(ArenaId arenaId, usize allocSize)
{
    if (!__MemoryTracker_Enabled()) return;

    MemoryEvent* event = __MemoryTracker_PushEvent(MEMORY_EVENT_MEMORY_ALLOCATED);
    event->event.memoryAllocated = (MemoryEvent_MemoryAllocated) {
        .arenaId   = arenaId,
        .allocSize = allocSize,
    };

    __MemoryTracker_TryFlushEvents();
}

void MemoryTracker_FlushEvents(void)
{
    if (!__MemoryTracker_Enabled()) return;

    const usize count = __globalMemoryTracker.eventCount;
    for (usize i = 0; i < count; i++) {
        __MemoryEvent_Flush(
                &__globalMemoryTracker.events[i],
                __globalMemoryTracker.fdOutput);

        __globalMemoryTracker.events[i].type = MEMORY_EVENT_UNDEFINED;
        __globalMemoryTracker.events[i].event.undefined = (MemoryEvent_Undefined) {0};
    }

    __globalMemoryTracker.eventCount = 0;
}

MemoryTracker __globalMemoryTracker = {
    .eventCount  = 0,
    .fdOutput    = -1,
    .nextArenaId = 0,
};

#endif // WETMAN_ENABLE_MEMORY_TRACKER

