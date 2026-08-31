#ifndef WETMAN_UTILS_SLICE_H
#define WETMAN_UTILS_SLICE_H

#include <wetman/utils/macro.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/type.h>

#include <string.h>


#define SLICE_DECLARE(Prefix, Type)                                            \
    typedef struct {                                                           \
        Type*  data;                                                           \
        usize  len;                                                            \
        void*  __arenaPage;                                                    \
    } Prefix;                                                                  \
                                                                               \
    Prefix Prefix##_CreateEmpty(void);                                         \
    Prefix Prefix##_FromData(Type* data, usize len);                           \
    Prefix Prefix##_FromDataWithPage(Type* data, usize len, void* arenaPage);  \
    Prefix Prefix##_Concat(Prefix s1, Prefix s2, Arena* arena);                \
    Type*  Prefix##_At(const Prefix* slice, usize pos);


#define SLICE_IMPL(Prefix, Type)                                               \
    static i32 __##Prefix##_SamePage(const Prefix* s1, const Prefix* s2)       \
    {                                                                          \
        if (s1->__arenaPage == NULL || s2->__arenaPage == NULL) {              \
            return 0;                                                          \
        }                                                                      \
        return s1->__arenaPage == s2->__arenaPage;                             \
    }                                                                          \
                                                                               \
    MAYBE_UNUSED                                                               \
    static inline Prefix __##Prefix##_CopyOnArena(                             \
        const Type* data, usize len, Arena* arena)                             \
    {                                                                          \
        void* page = NULL;                                                     \
        Type* copy = (Type*)Arena_AllocWithPage(arena, len * sizeof(Type),     \
                                                &page);                        \
        memcpy(copy, data, len * sizeof(Type));                                \
        return Prefix##_FromDataWithPage(copy, len, page);                     \
    }                                                                          \
                                                                               \
    Prefix Prefix##_CreateEmpty(void)                                          \
    {                                                                          \
        Prefix s = { .data = NULL, .len = 0, .__arenaPage = NULL };            \
        return s;                                                              \
    }                                                                          \
                                                                               \
    Prefix Prefix##_FromData(Type* data, usize len)                            \
    {                                                                          \
        Prefix s = { .data = data, .len = len, .__arenaPage = NULL };          \
        return s;                                                              \
    }                                                                          \
                                                                               \
    Prefix Prefix##_FromDataWithPage(Type* data, usize len, void* arenaPage)   \
    {                                                                          \
        Prefix s = { .data = data, .len = len, .__arenaPage = arenaPage };     \
        return s;                                                              \
    }                                                                          \
                                                                               \
    Prefix Prefix##_Concat(Prefix s1, Prefix s2, Arena* arena)                 \
    {                                                                          \
        if (s1.len == 0 && s2.len == 0) {                                      \
            return Prefix##_CreateEmpty();                                     \
        }                                                                      \
        if (s2.len == 0) {                                                     \
            /* result is s1; skip the copy if s1 is already arena-backed */    \
            if (s1.__arenaPage != NULL) {                                      \
                return s1;                                                     \
            }                                                                  \
            return __##Prefix##_CopyOnArena(s1.data, s1.len, arena);           \
        }                                                                      \
        if (s1.len == 0) {                                                     \
            /* result is s2; skip the copy if s2 is already arena-backed */    \
            if (s2.__arenaPage != NULL) {                                      \
                return s2;                                                     \
            }                                                                  \
            return __##Prefix##_CopyOnArena(s2.data, s2.len, arena);           \
        }                                                                      \
                                                                               \
        const usize newLen = s1.len + s2.len;                                  \
        const usize s1DataLen = s1.len * sizeof(Type);                         \
        const usize s2DataLen = s2.len * sizeof(Type);                         \
        const Type* s1DataEnd = s1.data + s1.len;                              \
                                                                               \
        /* s1 and s2 already live sequentially on the same arena page */       \
        if (__##Prefix##_SamePage(&s1, &s2) && s1DataEnd == s2.data) {         \
            return Prefix##_FromDataWithPage(                                  \
                s1.data, newLen, s1.__arenaPage);                              \
        }                                                                      \
                                                                               \
        /* the arena's tail page hands out the next block exactly where s1 */  \
        /* ends on the same page, so appending s2 needs only one copy */       \
        Type* dataCandidate = (Type*)Arena_CanAllocOnSamePage(                 \
            arena, s2DataLen);                                                 \
        if (s1.__arenaPage != NULL && dataCandidate                            \
                && s1DataEnd == dataCandidate) {                               \
            Type* data = (Type*)Arena_Alloc(arena, s2DataLen);                 \
            memcpy(data, s2.data, s2DataLen);                                  \
            return Prefix##_FromDataWithPage(                                  \
                s1.data, newLen, s1.__arenaPage);                              \
        }                                                                      \
                                                                               \
        /* general case: fresh contiguous allocation for both halves */        \
        void* page = NULL;                                                     \
        Type* data = (Type*)Arena_AllocWithPage(                               \
            arena, newLen * sizeof(Type), &page);                              \
        memcpy(data, s1.data, s1DataLen);                                      \
        memcpy(data + s1.len, s2.data, s2DataLen);                             \
        return Prefix##_FromDataWithPage(data, newLen, page);                  \
    }                                                                          \
                                                                               \
    Type* Prefix##_At(const Prefix* slice, usize pos)                          \
    {                                                                          \
        if (UNLIKELY(pos >= slice->len)) {                                     \
            return NULL;                                                       \
        }                                                                      \
        return &slice->data[pos];                                              \
    }

#endif // WETMAN_UTILS_SLICE_H
