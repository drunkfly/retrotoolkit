#ifndef COMMON_GC_H
#define COMMON_GC_H

#include "Common/Common.h"

class GCHeap;

class GCObject
{
public:
    GCObject();
    virtual ~GCObject();

    GCHeap* heap() const noexcept { return mHeap; }

    void* operator new(size_t size, GCHeap* heap);

    void operator delete(void* ptr) = delete;
    void operator delete(void* ptr, GCHeap* heap) = delete;

protected:
    void registerFinalizer();

private:
    GCHeap* mHeap;
    GCObject* mNext;

    DISABLE_COPY(GCObject);
    friend class GCHeap;
};

class GCHeap
{
public:
    GCHeap();
    ~GCHeap();

    void* alloc(size_t size);

    char* allocString(const char* str);
    char* allocString(const char* str, size_t len);

private:
    enum { ArenaSize = 1048576 };
    struct Arena
    {
        Arena* prev;
        size_t bytesLeft;
        char data[ArenaSize];
    };

    GCObject* mFinalizers;
    Arena* mArena;

    DISABLE_COPY(GCHeap);
    friend class GCObject;
};

#endif
