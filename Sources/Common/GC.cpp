#include "GC.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GCObject::GCObject()
    : mNext(nullptr)
{
}

GCObject::~GCObject()
{
}

void GCObject::registerFinalizer()
{
    if (!mNext) {
        mNext = mHeap->mFinalizers;
        mHeap->mFinalizers = this;
    }
}

void* GCObject::operator new(size_t size, GCHeap* heap)
{
    void* ptr = heap->alloc(size);

    GCObject* obj = reinterpret_cast<GCObject*>(ptr);
    obj->mHeap = heap;

    return ptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GCHeap::GCHeap()
    : mFinalizers(nullptr)
    , mArena(nullptr)
{
}

GCHeap::~GCHeap()
{
    for (GCObject* obj = mFinalizers; obj; obj = obj->mNext)
        obj->~GCObject();

    for (;;) {
        Arena* arena = mArena;
        if (!arena)
            break;
        mArena = arena->prev;
        free(arena);
    }
}

void* GCHeap::alloc(size_t size)
{
    Arena* arena;

    if (size == 0)
        size = 1;

    if (size >= ArenaSize / 2) {
        arena = reinterpret_cast<Arena*>(malloc(offsetof(Arena, data) + size));
        if (!arena)
            throw std::bad_alloc();

        if (!mArena)
            mArena = arena;
        else {
            arena->prev = mArena->prev;
            mArena->prev = arena;
        }

        arena->bytesLeft = 0;
        return arena->data;
    }

    arena = mArena;
    if (!arena || arena->bytesLeft < size) {
        arena = reinterpret_cast<Arena*>(malloc(sizeof(Arena)));
        if (!arena)
            throw std::bad_alloc();

        arena->bytesLeft = ArenaSize;
        arena->prev = mArena;
        mArena = arena;
    }

    arena->bytesLeft -= size;
    return &arena->data[arena->bytesLeft];
}

char* GCHeap::allocString(const char* str)
{
    size_t len = strlen(str) + 1;
    char* copy = reinterpret_cast<char*>(alloc(len));
    memcpy(copy, str, len);
    return copy;
}

char* GCHeap::allocString(const char* str, size_t len)
{
    char* copy = reinterpret_cast<char*>(alloc(len + 1));
    memcpy(copy, str, len);
    copy[len] = 0;
    return copy;
}
