#include "DebugInformation.h"

DebugInformation::DebugInformation()
{
}

DebugInformation::~DebugInformation()
{
}

void DebugInformation::addEmptySpace(int64_t start, int64_t size)
{
    mSections.emplace_back(createEmptySpace(start, size));
}

void DebugInformation::addSection(std::string name, int64_t start,
    Compression compression, int64_t uncompressedSize, std::optional<int64_t> compressedSize)
{
    mSections.emplace_back(createSection(std::move(name),
        start, compression, uncompressedSize, std::move(compressedSize)));
}

DebugInformation::Section DebugInformation::createEmptySpace(int64_t start, int64_t size)
{
    Section section;
    section.name = "[unused]";
    section.compression = Compression::None;
    section.startAddress = start;
    section.uncompressedSize = size;
    section.compressedSize = size;
    section.isEmptySpace = true;
    return section;
}

DebugInformation::Section DebugInformation::createSection(std::string name, int64_t start,
    Compression compression, int64_t uncompressedSize, std::optional<int64_t> compressedSize)
{
    Section section;
    section.name = std::move(name);
    section.compression = compression;
    section.startAddress = start;
    section.uncompressedSize = uncompressedSize;
    section.compressedSize = std::move(compressedSize);
    section.isEmptySpace = false;
    return section;
}
