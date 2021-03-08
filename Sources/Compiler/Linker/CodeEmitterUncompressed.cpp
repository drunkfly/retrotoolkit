#include "CodeEmitterUncompressed.h"

CodeEmitterUncompressed::CodeEmitterUncompressed()
{
}

CodeEmitterUncompressed::~CodeEmitterUncompressed()
{
}

void CodeEmitterUncompressed::clear()
{
    mBytes.clear();
}

void CodeEmitterUncompressed::addSectionDebugInfo(std::string name, int64_t start,
    Compression compression, int64_t uncompressedSize, std::optional<int64_t> compressedSize)
{
    DebugInformation::Section section;
    section.name = std::move(name);
    section.compression = compression;
    section.startAddress = start;
    section.uncompressedSize = uncompressedSize;
    section.compressedSize = compressedSize;
    mSections.emplace_back(std::move(section));
}

void CodeEmitterUncompressed::emitByte(SourceLocation* location, uint8_t byte)
{
    mBytes.emplace_back(Byte{ location, byte });
}

void CodeEmitterUncompressed::emitBytes(SourceLocation* location, const uint8_t* bytes, size_t count)
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

void CodeEmitterUncompressed::emitBytes(const Byte* bytes, size_t count)
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

void CodeEmitterUncompressed::copyTo(CodeEmitter* target) const
{
    target->emitBytes(mBytes.data(), mBytes.size());

    for (const auto& section : mSections) {
        target->addSectionDebugInfo(section.name, section.startAddress,
            section.compression, section.uncompressedSize, section.compressedSize);
    }
}
