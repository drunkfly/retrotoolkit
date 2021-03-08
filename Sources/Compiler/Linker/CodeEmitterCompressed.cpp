#include "CodeEmitterCompressed.h"
#include "Compiler/Compression/Compressor.h"
#include "Compiler/CompilerError.h"

CodeEmitterCompressed::CodeEmitterCompressed(std::unique_ptr<Compressor> compressor)
    : mLocation(nullptr)
    , mCompressor(std::move(compressor))
    , mCompressed(false)
{
}

CodeEmitterCompressed::~CodeEmitterCompressed()
{
}

size_t CodeEmitterCompressed::compressedSize() const
{
    if (!mCompressed)
        throw CompilerError(mLocation, "internal compiler error: compressed size is not known at this point.");
    return mCompressedBytes.size();
}

const uint8_t* CodeEmitterCompressed::compressedData() const
{
    if (!mCompressed)
        throw CompilerError(mLocation, "internal compiler error: compressed data is not available at this point.");
    return mCompressedBytes.data();
}

void CodeEmitterCompressed::clear()
{
    mCompressedBytes.clear();
    mUncompressedBytes.clear();
    mCompressed = false;
}

void CodeEmitterCompressed::addSectionDebugInfo(std::string name, int64_t start,
    Compression compression, int64_t uncompressedSize, std::optional<int64_t> compressedSize)
{
    if (mSection) {
        throw CompilerError(mLocation,
            "internal compiler error: only one section is allowed in compressed code emitter.");
    }

    if (compression != Compression::None || compressedSize.has_value()) {
        throw CompilerError(mLocation,
            "internal compiler error: attempted to write already compressed data to compressed code emitter.");
    }

    mSection = std::make_unique<DebugInformation::Section>();
    mSection->name = std::move(name);
    mSection->startAddress = start;
    mSection->compression = compression;
    mSection->uncompressedSize = uncompressedSize;
    mSection->compressedSize = compressedSize;
}

void CodeEmitterCompressed::emitByte(SourceLocation* location, uint8_t byte)
{
    if (mCompressed)
        throw CompilerError(mLocation, "internal compiler error: attempted to write more data after compression.");
    if (!mLocation)
        mLocation = location;

    mUncompressedBytes.emplace_back(byte);
}

void CodeEmitterCompressed::emitBytes(SourceLocation* location, const uint8_t* bytes, size_t count)
{
    if (mCompressed)
        throw CompilerError(mLocation, "internal compiler error: attempted to write more data after compression.");
    if (!mLocation)
        mLocation = location;

    size_t curSize = mUncompressedBytes.size();
    size_t newSize = curSize + count;
    size_t curCapacity = mUncompressedBytes.capacity();
    if (newSize > curCapacity) {
        size_t newCapacity = (curCapacity == 0 ? 16 : curCapacity * 2);
        if (newCapacity < newSize)
            newCapacity = newSize * 2;
        mUncompressedBytes.reserve(newCapacity);
    }

    mUncompressedBytes.insert(mUncompressedBytes.end(), bytes, bytes + count);
}

void CodeEmitterCompressed::emitBytes(const Byte* bytes, size_t count)
{
    if (mCompressed)
        throw CompilerError(mLocation, "internal compiler error: attempted to write more data after compression.");
    if (!mLocation && count > 0)
        mLocation = bytes[0].location;

    size_t curSize = mUncompressedBytes.size();
    size_t newSize = curSize + count;
    size_t curCapacity = mUncompressedBytes.capacity();
    if (newSize > curCapacity) {
        size_t newCapacity = (curCapacity == 0 ? 16 : curCapacity * 2);
        if (newCapacity < newSize)
            newCapacity = newSize * 2;
        mUncompressedBytes.reserve(newCapacity);
    }

    const Byte* end = bytes + count;
    while (bytes < end)
        mUncompressedBytes.emplace_back((*bytes++).value);
}

void CodeEmitterCompressed::compress()
{
    if (mCompressed)
        throw CompilerError(mLocation, "internal compiler error: data is already compressed.");

    mCompressor->compress(mLocation, std::move(mUncompressedBytes), mCompressedBytes);
    mCompressed = true;
}

void CodeEmitterCompressed::copyTo(CodeEmitter* target) const
{
    if (!mCompressed)
        throw CompilerError(mLocation, "internal compiler error: compressed data is not available at this point.");

    target->emitBytes(mLocation, mCompressedBytes.data(), mCompressedBytes.size());

    if (mSection) {
        target->addSectionDebugInfo(mSection->name, mSection->startAddress,
            mCompressor->compression(), mSection->uncompressedSize, mCompressedBytes.size());
    }
}
