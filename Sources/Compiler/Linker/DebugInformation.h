#ifndef COMPILER_LINKER_DEBUGINFORMATION_H
#define COMPILER_LINKER_DEBUGINFORMATION_H

#include "Compiler/Compression/Compression.h"

class DebugInformation
{
public:
    struct Section
    {
        std::string name;
        Compression compression;
        int64_t startAddress;
        int64_t uncompressedSize;
        std::optional<int64_t> compressedSize;
    };

    DebugInformation();
    ~DebugInformation();

    const std::vector<Section>& sections() const { return mSections; }

    void addSection(std::string name, int64_t start,
        Compression compression, int64_t uncompressedSize, std::optional<int64_t> compressedSize);

private:
    std::vector<Section> mSections;

    DISABLE_COPY(DebugInformation);
};

#endif
