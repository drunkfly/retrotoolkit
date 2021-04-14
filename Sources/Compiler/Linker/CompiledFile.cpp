#include "CompiledFile.h"
#include "Compiler/Linker/DebugInformation.h"

CompiledFile::CompiledFile(SourceLocation* location, std::string name, std::unique_ptr<DebugInformation> debugInfo)
    : mLocation(location)
    , mName(std::move(name))
    , mDebugInfo(std::move(debugInfo))
    , mLoadAddress(0)
    , mUsedByBasic(false)
{
    registerFinalizer();
}

CompiledFile::~CompiledFile()
{
}

std::unique_ptr<DebugInformation> CompiledFile::takeDebugInfo()
{
    std::unique_ptr<DebugInformation> info{std::move(mDebugInfo)};
    return info;
}

void CompiledFile::addEmptySpaceDebugInfo(int64_t start, int64_t size)
{
    mDebugInfo->addEmptySpace(start, size);
}

void CompiledFile::addSectionDebugInfo(std::string name, int64_t start,
    Compression compression, int64_t uncompressedSize, std::optional<int64_t> compressedSize)
{
    mDebugInfo->addSection(std::move(name), start, compression, uncompressedSize, compressedSize);
}
