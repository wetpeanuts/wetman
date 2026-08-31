#ifndef WETMAN_UTILS_MEM_ARENA_H
#define WETMAN_UTILS_MEM_ARENA_H

#include <stdlib.h>


typedef struct __ArenaPageHeader __ArenaPageHeader;

typedef struct Arena {
    __ArenaPageHeader* __headPage;
    __ArenaPageHeader* __tailPage;
} Arena;

Arena Arena_New(void);
Arena Arena_WithPageCapacity(size_t capacity);

void* Arena_Alloc(Arena* arena, size_t size);
// Fills page with pointer to arena page where the memory was allocated.
// The pointer can be used as unique id of arena page e.g. for allocation optimizations.
void* Arena_AllocWithPage(Arena* arena, size_t size, void** page);
void* Arena_CanAllocOnSamePage(Arena* arena, size_t size);
void Arena_Reset(Arena* arena);
void Arena_Free(Arena* arena);
int Arena_IsValid(const Arena* arena);

#endif // WETMAN_UTILS_MEM_ARENA_H
