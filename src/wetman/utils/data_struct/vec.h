#ifndef WETMAN_UTILS_VEC_H
#define WETMAN_UTILS_VEC_H

#include <wetman/utils/mem/arena.h>
#include <wetman/utils/type.h>


typedef struct {
    usize  len;
    usize  cap;

    void*  __data;
    usize  __itemSize;
    Arena* __arena;
} Vec;

Vec Vec_New(usize itemSize, Arena* arena);
Vec Vec_WithCapacity(usize itemSize, usize cap, Arena* arena);

// Returns 1 on success, 0 on failure
i32 Vec_Push(Vec* vec, void* item, usize itemSize);
void* Vec_At(Vec* vec, usize pos);


#define VEC_NEW(type, arena) Vec_New(sizeof(type), arena)
#define VEC_WITH_CAPACITY(type, cap, arena) Vec_WithCapacity(sizeof(type), cap, arena)

#define VEC_PUSH(type, vec, item) Vec_Push(vec, item, sizeof(type))
#define VEC_AT(type, vec, pos) (type*)Vec_At(vec, pos)

#endif // WETMAN_UTILS_VEC_H
