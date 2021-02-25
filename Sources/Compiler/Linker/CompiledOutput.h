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

private:
    std::string mName;
    size_t mLoadAddress;

    DISABLE_COPY(CompiledFile);
};

class CompiledOutput : public GCObject
{
public:
    CompiledOutput();
    ~CompiledOutput() override;

    CompiledFile* getOrAddFile(const std::string& name);

private:
    std::unordered_map<std::string, CompiledFile*> mFiles;

    DISABLE_COPY(CompiledOutput);
};

#endif
