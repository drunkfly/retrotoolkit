#include "DebugInformation.h"

DebugInformation::DebugInformation()
{
}

DebugInformation::~DebugInformation()
{
}

void DebugInformation::addSection(std::string name, int64_t start,
    Compression compression, int64_t uncompressedSize, std::optional<int64_t> compressedSize)
{
    Section section;
    section.name = std::move(name);
    section.compression = compression;
    section.startAddress = start;
    section.uncompressedSize = uncompressedSize;
    section.compressedSize = compressedSize;
    mSections.emplace_back(std::move(section));
}
