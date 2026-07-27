#ifndef WETMAN_UTILS_MEM_ARENA_H
#define WETMAN_UTILS_MEM_ARENA_H

#include <stdlib.h>


typedef struct __ArenaPageHeader __ArenaPageHeader;

struct __ArenaPageHeader {
    size_t             __size;
    size_t             __capacity;
    __ArenaPageHeader* __prevPage;
};

typedef struct {
    __ArenaPageHeader* __headPage;
    __ArenaPageHeader* __tailPage;
} Arena;

Arena Arena_New(void);
Arena Arena_WithPageCapacity(size_t capacity);
void* Arena_Alloc(Arena* arena, size_t size);
void* Arena_CanAllocOnSamePage(Arena* arena, size_t size);
void Arena_Reset(Arena* arena);
void Arena_Free(Arena* arena);

#endif // WETMAN_UTILS_MEM_ARENA_H
