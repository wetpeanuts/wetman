#ifndef WETMAN_UTILS_VEC_H
#define WETMAN_UTILS_VEC_H

#include <wetman/utils/mem/arena.h>
#include <wetman/utils/macro.h>
#include <wetman/utils/type.h>

#include <string.h>


#define VEC_DECLARE(Prefix, Type)                                               \
    typedef struct {                                                           \
        Type*  data;                                                           \
        usize  len;                                                            \
        usize  cap;                                                            \
        void*  __arena;                                                        \
    } Prefix;                                                                  \
                                                                               \
    Prefix Prefix##_New(Arena* arena);                                          \
    Prefix Prefix##_WithCapacity(usize cap, Arena* arena);                      \
    i32    Prefix##_Push(Prefix* vec, Type* item);                              \
    Type*  Prefix##_At(const Prefix* vec, usize pos);


#define VEC_IMPL(Prefix, Type)                                                  \
    Prefix Prefix##_New(Arena* arena)                                           \
    {                                                                          \
        Prefix vec = {                                                         \
            .len     = 0,                                                      \
            .cap     = 2,                                                      \
            .data    = (Type*)Arena_Alloc(arena, 2 * sizeof(Type)),             \
            .__arena = arena,                                                  \
        };                                                                     \
        return vec;                                                            \
    }                                                                          \
                                                                               \
    Prefix Prefix##_WithCapacity(usize cap, Arena* arena)                       \
    {                                                                          \
        Prefix vec = {                                                         \
            .len     = 0,                                                      \
            .cap     = cap,                                                    \
            .data    = (Type*)Arena_Alloc(arena, cap * sizeof(Type)),           \
            .__arena = arena,                                                  \
        };                                                                     \
        return vec;                                                            \
    }                                                                          \
                                                                               \
    i32 Prefix##_Push(Prefix* vec, Type* item)                                  \
    {                                                                          \
        if (UNLIKELY(vec->len == vec->cap)) {                                   \
            usize newCap = vec->cap * 2;                                        \
            Type* newData = (Type*)Arena_Alloc(vec->__arena,                    \
                                               newCap * sizeof(Type));          \
            if (UNLIKELY(newData == NULL)) {                                   \
                return 0;                                                      \
            }                                                                  \
            memcpy(newData, vec->data, vec->len * sizeof(Type));                \
            vec->data = newData;                                               \
            vec->cap  = newCap;                                                \
        }                                                                      \
                                                                               \
        vec->data[vec->len++] = *item;                                         \
        return 1;                                                              \
    }                                                                          \
                                                                               \
    Type* Prefix##_At(const Prefix* vec, usize pos)                             \
    {                                                                          \
        if (UNLIKELY(pos >= vec->len)) {                                       \
            return NULL;                                                       \
        }                                                                      \
        return &vec->data[pos];                                                \
    }

#endif // WETMAN_UTILS_VEC_H
