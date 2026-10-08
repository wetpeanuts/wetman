#ifndef WETMAN_UTILS_MEM_MEMORY_TRACKER_READER_H
#define WETMAN_UTILS_MEM_MEMORY_TRACKER_READER_H

#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/mem/memory_tracker.h>


// Reads a memory-event dump (the text lines produced by
// MemoryTracker_FlushEvents) from `path` into `out`. The event array is
// allocated from `arena`.
// Returns 0 on success, -1 if the file cannot be opened or read, or if any
// line does not match a known MemoryEvent line format.
i32 MemoryTrackerReader_ReadFromFile(Str path, Arena* arena, MemoryEventList* out);

// Verifies that every arena born via ARENA_CREATED was later freed via
// ARENA_FREED. Also rejects duplicate creates, frees/resets/allocations of
// arenas that are not alive, and double frees. State bookkeeping is allocated
// from `arena`.
// Returns TRUE if the dump is consistent and leak-free, FALSE otherwise.
i32 MemoryTrackerReader_VerifyAllArenasFreed(const MemoryEventList* list, Arena* arena);

#endif // WETMAN_UTILS_MEM_MEMORY_TRACKER_READER_H
