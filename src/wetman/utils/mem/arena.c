#include <wetman/utils/mem/arena.h>

#include <wetman/utils/macro.h>

#include <assert.h>
#include <stdio.h>


#define __ARENA_DEFAULT_PAGE_CAPACITY 4096


struct __ArenaPageHeader {
    size_t             __size;
    size_t             __capacity;
    __ArenaPageHeader* __prevPage;
};


__ArenaPageHeader* __allocPage(size_t capacity)
{
    assert(capacity > 0);

    __ArenaPageHeader* page = (__ArenaPageHeader*)malloc(sizeof(__ArenaPageHeader) + capacity);
    assert(page);

    page->__size = 0;
    page->__capacity = capacity;
    page->__prevPage = NULL;

    return page;
}

void* __pageData(__ArenaPageHeader* page, size_t offset)
{
    // TODO: consider alignment
    return (void*)(((unsigned char*)(page + 1)) + offset);
}

Arena Arena_New(void)
{
    return Arena_WithPageCapacity(__ARENA_DEFAULT_PAGE_CAPACITY);
}

Arena Arena_WithPageCapacity(size_t capacity)
{
    __ArenaPageHeader* headPage = __allocPage(capacity);

    Arena arena = {
        .__headPage = headPage,
        .__tailPage = headPage,
    };

    return arena;
}

void* Arena_Alloc(Arena* arena, size_t size)
{
    __ArenaPageHeader* tailPage = arena->__tailPage;
    assert(tailPage);

    const size_t newSize = tailPage->__size + size;
    if (newSize <= tailPage->__capacity) {
        void* data = __pageData(tailPage, tailPage->__size);
        tailPage->__size = newSize;
        return data;
    }

    __ArenaPageHeader* newTailPage = __allocPage(MAX(arena->__headPage->__capacity, size));

    void* data = __pageData(newTailPage, 0);
    newTailPage->__size = size;
    newTailPage->__prevPage = arena->__tailPage;
    arena->__tailPage = newTailPage;
    return data;
}

void* Arena_AllocWithPage(Arena* arena, size_t size, void** page)
{
    __ArenaPageHeader* tailPage = arena->__tailPage;
    assert(tailPage);

    const size_t newSize = tailPage->__size + size;
    if (newSize <= tailPage->__capacity) {
        void* data = __pageData(tailPage, tailPage->__size);
        tailPage->__size = newSize;
        *page = tailPage;
        return data;
    }

    __ArenaPageHeader* newTailPage = __allocPage(MAX(arena->__headPage->__capacity, size));

    void* data = __pageData(newTailPage, 0);
    newTailPage->__size = size;
    newTailPage->__prevPage = arena->__tailPage;
    arena->__tailPage = newTailPage;
    *page = newTailPage;
    return data;
}

void* Arena_CanAllocOnSamePage(Arena* arena, size_t size)
{
    __ArenaPageHeader* tailPage = arena->__tailPage;
    assert(tailPage);

    if (tailPage->__size + size <= tailPage->__capacity) {
        return __pageData(tailPage, tailPage->__size);
    }

    return NULL;
}

void Arena_Reset(Arena* arena)
{
    assert(arena->__tailPage);

    // Clean all pages except the first one
    __ArenaPageHeader* currTailPage = arena->__tailPage;
    while (currTailPage->__prevPage) {
        __ArenaPageHeader* pageToDelete = currTailPage;
        currTailPage = currTailPage->__prevPage;
        free(pageToDelete);
    }

    currTailPage->__size = 0;
    arena->__tailPage = currTailPage;
}

void Arena_Free(Arena* arena)
{
    // Clean all pages except the first one
    Arena_Reset(arena);

    // Clean the last page, nullify head ptr
    free(arena->__headPage);
    arena->__headPage = NULL;
    arena->__tailPage = NULL;
}

int Arena_IsValid(const Arena* arena)
{
    return arena->__headPage && arena->__tailPage;
}

