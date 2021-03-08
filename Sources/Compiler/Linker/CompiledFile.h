#ifndef COMPILER_LINKER_COMPILEDFILE_H
#define COMPILER_LINKER_COMPILEDFILE_H

#include "Common/Common.h"
#include "Common/GC.h"
#include "Compiler/Linker/CodeEmitterUncompressed.h"
#include <memory>
#include <string>

class DebugInformation;

class CompiledFile final : public GCObject, public CodeEmitterUncompressed
{
public:
    CompiledFile(std::string name, std::unique_ptr<DebugInformation> debugInfo);
    ~CompiledFile() override;

    const std::string& name() const { return mName; }

    DebugInformation* debugInfo() const { return mDebugInfo.get(); }
    std::unique_ptr<DebugInformation> takeDebugInfo();

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
    bool mUsedByBasic;

    DISABLE_COPY(CompiledFile);
};

#endif
