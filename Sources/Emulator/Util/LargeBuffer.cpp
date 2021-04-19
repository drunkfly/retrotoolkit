#include "LargeBuffer.h"

LargeBuffer::LargeBuffer(size_t size)
    : mBuffer(new uint8_t[size])
    , mSize(0)
    , mReserved(size)
{
}

LargeBuffer::~LargeBuffer()
{
}

uint8_t LargeBuffer::operator[](size_t offset) const
{
    ensureLength(offset + 1);
    return mBuffer[offset];
}

uint8_t& LargeBuffer::operator[](size_t offset)
{
    ensureLength(offset + 1);
    return mBuffer[offset];
}

void LargeBuffer::readAt(size_t offset, void* dst, size_t count) const
{
    if (offset + count > mSize)
        throw std::runtime_error("Unexpected end of data.");
    memcpy(dst, &mBuffer[offset], count);
}

void LargeBuffer::readFile(const char* path)
{
    FILE* f = fopen(path, "rb");
    if (!f) {
        const char* err = strerror(errno);
        std::stringstream ss;
        ss << "Unable to open file \"" << path << "\": " << err;
        throw std::runtime_error(ss.str());
    }

    mSize = fread(mBuffer.get(), 1, mReserved, f);
    if (ferror(f)) {
        const char* err = strerror(errno);
        fclose(f);
        std::stringstream ss;
        ss << "Unable to read file \"" << path << "\": " << err;
        throw std::runtime_error(ss.str());
    }

    fclose(f);
}

void LargeBuffer::ensureLength(size_t needed) const
{
    if (mSize < needed)
        throw std::runtime_error("Unexpected end of data.");
}
