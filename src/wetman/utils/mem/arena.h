#ifndef WETMAN_UTILS_MEM_ARENA_H
#define WETMAN_UTILS_MEM_ARENA_H

#include <stdlib.h>


typedef struct {
    void* data;
    size_t size;
    size_t capacity;
} Arena;

Arena Arena_New(void);
void* Arena_Alloc(Arena* arena, size_t size);
void Arena_Reset(Arena* arena);
void Arena_Free(Arena* arena);

#endif // WETMAN_UTILS_MEM_ARENA_H
