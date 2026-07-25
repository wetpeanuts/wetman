#include <wetman/utils/mem/arena.h>

#include <wetman/utils/macro.h>

#include <assert.h>
#include <stdio.h>


#define __ARENA_DEFAULT_PAGE_SIZE 4096


__ArenaPageHeader* __allocPage(size_t size)
{
    assert(size > 0);

    __ArenaPageHeader* page = (__ArenaPageHeader*)malloc(sizeof(__ArenaPageHeader) + size);
    assert(page);

    page->__size = 0;
    page->__prevPage = NULL;

    return page;
}

void* __pageData(__ArenaPageHeader* page)
{
    return (void*)(page + 1);
}


Arena Arena_New(void)
{
    __ArenaPageHeader* headPage = __allocPage(__ARENA_DEFAULT_PAGE_SIZE);

    Arena arena = {
        .__headPage = headPage,
    };

    return arena;
}

void* Arena_Alloc(Arena* arena, size_t size)
{
    __ArenaPageHeader* headPage = arena->__headPage;
    assert(headPage);

    const size_t newSize = headPage->__size + size;
    // TODO: compare with capacity
    if (newSize <= __ARENA_DEFAULT_PAGE_SIZE) {
        void* data = __pageData(headPage) + headPage->__size;
        headPage->__size = newSize;
        return data;
    }

    __ArenaPageHeader* newHeadPage = __allocPage(MAX(__ARENA_DEFAULT_PAGE_SIZE, size));

    void* data = __pageData(newHeadPage);
    newHeadPage->__size = size;
    return data;
}

void* Arena_CanAllocOnSamePage(Arena* arena, size_t size)
{
    __ArenaPageHeader* headPage = arena->__headPage;
    assert(headPage);

    // TODO: compare with capacity
    if (headPage->__size + size <= __ARENA_DEFAULT_PAGE_SIZE) {
        return __pageData(headPage) + headPage->__size;
    }

    return NULL;
}

void Arena_Reset(Arena* arena)
{
    assert(arena->__headPage);

    // Clean all pages except the first one
    __ArenaPageHeader* currPage = arena->__headPage;
    while (currPage->__prevPage) {
        __ArenaPageHeader* pageToDelete = currPage;
        currPage = currPage->__prevPage;
        free(pageToDelete);
    }

    currPage->__size = 0;
    arena->__headPage = currPage;
}

void Arena_Free(Arena* arena)
{
    // Clean all pages except the first one
    Arena_Reset(arena);

    // Clean the last page, nullify head ptr
    free(arena->__headPage);
    arena->__headPage = NULL;
}
