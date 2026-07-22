#include <wetman/utils/mem/arena.h>

#include <wetman/utils/macro.h>

#include <stdio.h>


static const size_t ARENA_DEFAULT_CAPACITY = 4096;

Arena Arena_New(void)
{
    Arena arena = {
        .data = malloc(ARENA_DEFAULT_CAPACITY),
        .size = 0,
        .capacity = ARENA_DEFAULT_CAPACITY,
    };

    if (UNLIKELY(!arena.data)) {
        fprintf(stderr, "Error: Failed to allocate arena");
    }

    return arena;
}

void* Arena_Alloc(Arena* arena, size_t size)
{
    if (UNLIKELY(!arena->data)) {
        fprintf(stderr, "Error: Attempt to allocate on invalid arena");
        return NULL;
    }

    const size_t newSize = arena->size + size;
    if (newSize <= arena->capacity) {
        void* memPtr = arena->data + arena->size;
        arena->size = newSize;
        return memPtr;
    }

    arena->capacity = ARENA_DEFAULT_CAPACITY * ((newSize / ARENA_DEFAULT_CAPACITY) + 1);

    void* newData = realloc(arena->data, newSize);
    if (UNLIKELY(!newData)) {
        fprintf(stderr, "Error: Failed to reallocate arena");
        Arena_Free(arena);
        return NULL;
    }

    arena->data = newData;
    void* memPtr = newData + arena->size;
    arena->size = newSize;
    return memPtr;
}

void Arena_Reset(Arena* arena)
{
    arena->size = 0;
}

void Arena_Free(Arena* arena)
{
    free(arena->data);
    arena->data = NULL;
    arena->size = 0;
    arena->capacity = 0;
}
