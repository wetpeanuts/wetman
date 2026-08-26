#include <wetman/utils/data_struct/vec.h>

#include <wetman/utils/macro.h>

#include <string.h>


static inline void* __Vec_DataWithOffset(Vec* vec, usize offset)
{
    return (void*)((char*)vec->__data + offset);
}


Vec Vec_New(usize itemSize, Arena* arena)
{
    Vec vec = {
        .len        = 0,
        .cap        = 2,
        .__data     = Arena_Alloc(arena, 2 * itemSize),
        .__itemSize = itemSize,
        .__arena    = arena,
    };

    return vec;
}

Vec Vec_WithCapacity(usize itemSize, usize cap, Arena* arena)
{
    Vec vec = {
        .len        = 0,
        .cap        = cap,
        .__data     = Arena_Alloc(arena, cap * itemSize),
        .__itemSize = itemSize,
        .__arena    = arena,
    };

    return vec;
}

i32 Vec_Push(Vec* vec, void* item, usize itemSize)
{
    if (UNLIKELY(itemSize != vec->__itemSize)) {
        return 0;
    }

    if (vec->len == vec->cap) {
        vec->cap *= 2;
        void* newData = Arena_Alloc(vec->__arena, vec->cap * vec->__itemSize);
        
        if (UNLIKELY(newData == NULL)) {
            return 0;
        }

        memcpy(newData, vec->__data, vec->len * vec->__itemSize);
        vec->__data = newData;
    }

    void* dest = __Vec_DataWithOffset(vec, (vec->len++) * itemSize);
    memcpy(dest, item, itemSize);

    return 1;
}

void* Vec_At(Vec* vec, usize pos)
{
    if (UNLIKELY(pos >= vec->len)) {
        return NULL;
    }

    return __Vec_DataWithOffset(vec, pos * vec->__itemSize);
}

