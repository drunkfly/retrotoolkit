#ifndef COMPILER_LINKER_COMPILEDOUTPUT_H
#define COMPILER_LINKER_COMPILEDOUTPUT_H

#include "Common/Common.h"
#include "Common/GC.h"
#include "Compiler/Linker/CodeEmitter.h"
#include <vector>
#include <unordered_map>

class CompiledFile : public GCObject, public UncompressedCodeEmitter
{
public:
    explicit CompiledFile(std::string name);
    ~CompiledFile() override;

    size_t loadAddress() const { return mLoadAddress; }

    bool isUsedByBasic() const { return mUsedByBasic; }
    void setUsedByBasic() { mUsedByBasic = true; }

private:
    std::string mName;
    size_t mLoadAddress;
    bool mUsedByBasic;

    DISABLE_COPY(CompiledFile);
};

class CompiledOutput : public GCObject
{
public:
    CompiledOutput();
    ~CompiledOutput() override;

    const std::vector<CompiledFile*>& files() const { return mFileList; }

    CompiledFile* getFile(const std::string& name);
    CompiledFile* getOrAddFile(const std::string& name);

private:
    std::unordered_map<std::string, CompiledFile*> mFiles;
    std::vector<CompiledFile*> mFileList;

    DISABLE_COPY(CompiledOutput);
};

#endif
