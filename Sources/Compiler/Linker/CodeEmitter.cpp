#include "CodeEmitter.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CodeEmitter::CodeEmitter()
{
}

CodeEmitter::~CodeEmitter()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UncompressedCodeEmitter::UncompressedCodeEmitter()
{
}

UncompressedCodeEmitter::~UncompressedCodeEmitter()
{
}

void UncompressedCodeEmitter::clear()
{
    mBytes.clear();
}

void UncompressedCodeEmitter::emitByte(SourceLocation* location, uint8_t byte)
{
    mBytes.emplace_back(Byte{ location, byte });
}

void UncompressedCodeEmitter::emitBytes(SourceLocation* location, const uint8_t* bytes, size_t count)
{
    size_t curSize = mBytes.size();
    size_t newSize = curSize + count;
    size_t curCapacity = mBytes.capacity();
    if (newSize > curCapacity) {
        size_t newCapacity = (curCapacity == 0 ? 16 : curCapacity * 2);
        if (newCapacity < newSize)
            newCapacity = newSize * 2;
        mBytes.reserve(newCapacity);
    }

    const uint8_t* end = bytes + count;
    while (bytes < end)
        mBytes.emplace_back(Byte{ location, *bytes++ });
}

void UncompressedCodeEmitter::emitBytes(const Byte* bytes, size_t count)
{
    size_t curSize = mBytes.size();
    size_t newSize = curSize + count;
    size_t curCapacity = mBytes.capacity();
    if (newSize > curCapacity) {
        size_t newCapacity = (curCapacity == 0 ? 16 : curCapacity * 2);
        if (newCapacity < newSize)
            newCapacity = newSize * 2;
        mBytes.reserve(newCapacity);
    }

    mBytes.insert(mBytes.end(), bytes, bytes + count);
}

void UncompressedCodeEmitter::copyTo(CodeEmitter* target) const
{
    target->emitBytes(mBytes.data(), mBytes.size());
}
