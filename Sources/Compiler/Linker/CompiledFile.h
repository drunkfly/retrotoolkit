#ifndef COMPILER_LINKER_COMPILEDFILE_H
#define COMPILER_LINKER_COMPILEDFILE_H

#include "Common/GC.h"
#include "Compiler/Linker/CodeEmitterUncompressed.h"

class DebugInformation;

class CompiledFile final : public GCObject, public CodeEmitterUncompressed
{
public:
    CompiledFile(SourceLocation* location, std::string name, std::unique_ptr<DebugInformation> debugInfo);
    ~CompiledFile() override;

    const std::string& name() const { return mName; }
    SourceLocation* location() const { return mLocation; }

    DebugInformation* debugInfo() const { return mDebugInfo.get(); }
    std::unique_ptr<DebugInformation> takeDebugInfo();

    void addEmptySpaceDebugInfo(int64_t start, int64_t size) override;
    void addSectionDebugInfo(std::string name, int64_t start,
        Compression compression, int64_t uncompressedSize, std::optional<int64_t> compressedSize) override;

    size_t loadAddress() const { return mLoadAddress; }
    void setLoadAddress(size_t address) { mLoadAddress = address; }

    bool isUsedByBasic() const { return mUsedByBasic; }
    void setUsedByBasic() { mUsedByBasic = true; }

private:
    std::string mName;
    std::unique_ptr<DebugInformation> mDebugInfo;
    size_t mLoadAddress;
    SourceLocation* mLocation;
    bool mUsedByBasic;

    DISABLE_COPY(CompiledFile);
};

#endif
