#ifndef EMULATOR_UTIL_LARGEBUFFER_H
#define EMULATOR_UTIL_LARGEBUFFER_H

#include "Emulator/Common.h"

class LargeBuffer
{
public:
    explicit LargeBuffer(size_t size);
    ~LargeBuffer();

    size_t size() const { return mSize; }

    uint8_t operator[](size_t offset) const;
    uint8_t& operator[](size_t offset);

    void readAt(size_t offset, void* dst, size_t count) const;

    void readFile(const char* path);

private:
    std::unique_ptr<uint8_t[]> mBuffer;
    size_t mSize;
    size_t mReserved;

    void ensureLength(size_t needed) const;

    DISABLE_COPY(LargeBuffer);
};

#endif
