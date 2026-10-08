#ifndef WETMAN_UTILS_MEM_MEMORY_TRACKER_H
#define WETMAN_UTILS_MEM_MEMORY_TRACKER_H

#include <wetman/utils/type.h>


#define MEMORY_TRACKER_MAX_EVENT_COUNT 1024

typedef struct Arena Arena;
typedef usize ArenaId;

typedef enum {
    MEMORY_EVENT_UNDEFINED,
    MEMORY_EVENT_ARENA_CREATED,
    MEMORY_EVENT_ARENA_FREED,
    MEMORY_EVENT_ARENA_RESET,
    MEMORY_EVENT_MEMORY_ALLOCATED,
} MemoryEventType;

typedef struct {
    i32 __dummy;
} MemoryEvent_Undefined;

typedef struct {
    ArenaId arenaId;
} MemoryEvent_ArenaCreated;

typedef struct {
    ArenaId arenaId;
} MemoryEvent_ArenaFreed;

typedef struct {
    ArenaId arenaId;
} MemoryEvent_ArenaReset;

typedef struct {
    ArenaId arenaId;
    usize   allocSize;
} MemoryEvent_MemoryAllocated;

typedef union {
    MemoryEvent_Undefined       undefined;
    MemoryEvent_ArenaCreated    arenaCreated;
    MemoryEvent_ArenaFreed      arenaFreed;
    MemoryEvent_ArenaReset      arenaReset;
    MemoryEvent_MemoryAllocated memoryAllocated;
} MemoryEventUnion;

typedef struct {
    MemoryEventType  type;
    i64              timestamp; // Unix epoch time in milliseconds
    MemoryEventUnion event;
} MemoryEvent;

typedef struct {
    MemoryEvent* events;
    usize        count;
} MemoryEventList;

typedef struct {
    MemoryEvent events[MEMORY_TRACKER_MAX_EVENT_COUNT];
    usize       eventCount;
    i32         fdOutput; // File descriptor to flush the events to
    usize       nextArenaId;
} MemoryTracker;

void MemoryTracker_Init(i32 fdOutput);
void MemoryTracker_ArenaCreated(Arena* arena);
void MemoryTracker_ArenaFreed(ArenaId arenaId);
void MemoryTracker_ArenaReset(ArenaId arenaId);
void MemoryTracker_MemoryAllocated(ArenaId arenaId, usize allocSize);
void MemoryTracker_FlushEvents(void);

extern MemoryTracker __globalMemoryTracker;

#endif // WETMAN_UTILS_MEM_MEMORY_TRACKER_H
