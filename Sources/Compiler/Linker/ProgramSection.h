#ifndef COMPILER_LINKER_PROGRAMSECTION_H
#define COMPILER_LINKER_PROGRAMSECTION_H

#include "Common/Common.h"
#include "Common/GC.h"
#include "Compiler/Compression/Compression.h"
#include <vector>
#include <string>

class Instruction;
class CodeEmitter;

class ProgramSection : public GCObject
{
public:
    explicit ProgramSection(std::string name);
    ~ProgramSection() override;

    const std::string& name() const { return mName; }

    size_t calculateSizeInBytes() const;

    void resolveLabels(size_t& address);
    void unresolveLabels();

    void addInstruction(Instruction* instruction);

    void emitCode(CodeEmitter* emitter, size_t baseAddress) const;

private:
    std::string mName;
    std::vector<Instruction*> mInstructions;

    DISABLE_COPY(ProgramSection);
};

#endif
