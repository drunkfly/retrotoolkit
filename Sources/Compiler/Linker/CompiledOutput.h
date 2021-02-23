#ifndef COMPILER_LINKER_COMPILEDOUTPUT_H
#define COMPILER_LINKER_COMPILEDOUTPUT_H

#include "Common/Common.h"
#include "Common/GC.h"
#include "Compiler/Linker/CodeEmitter.h"
#include <vector>
#include <unordered_map>

class CompiledFile : public GCObject, public CodeEmitter
{
public:
    struct Byte
    {
        SourceLocation* location;
        uint8_t value;
    };

    explicit CompiledFile(std::string name);
    ~CompiledFile() override;

    void emitByte(SourceLocation* location, uint8_t byte) override;

private:
    std::string mName;
    std::vector<Byte> mBytes;

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
