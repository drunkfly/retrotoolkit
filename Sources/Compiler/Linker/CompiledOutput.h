#ifndef COMPILER_LINKER_COMPILEDOUTPUT_H
#define COMPILER_LINKER_COMPILEDOUTPUT_H

#include "Compiler/Linker/CompiledFile.h"

class SourceLocation;

class CompiledOutput final : public GCObject
{
public:
    CompiledOutput();
    ~CompiledOutput() override;

    const std::vector<CompiledFile*>& files() const { return mFileList; }

    CompiledFile* getFile(const std::string& name);
    CompiledFile* addFile(SourceLocation* location, SourceLocation* nameLocation,
        const std::string& name, std::unique_ptr<DebugInformation> debugInfo);

private:
    std::unordered_map<std::string, CompiledFile*> mFiles;
    std::vector<CompiledFile*> mFileList;

    DISABLE_COPY(CompiledOutput);
};

#endif
