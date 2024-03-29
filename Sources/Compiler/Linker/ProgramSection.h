#ifndef COMPILER_LINKER_PROGRAMSECTION_H
#define COMPILER_LINKER_PROGRAMSECTION_H

#include "Common/GC.h"
#include "Compiler/Compression/Compression.h"

class Instruction;
class CodeEmitter;
class CompilerError;
class ISectionResolver;

class ProgramSection : public GCObject
{
public:
    explicit ProgramSection(std::string name);
    ~ProgramSection() override;

    const std::string& name() const { return mName; }

    bool calculateSizeInBytes(size_t& outSize,
        ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const;

    bool resolveLabels(size_t& address,
        ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError);
    void unresolveLabels();

    void addInstruction(Instruction* instruction);

    bool canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const;
    bool emitCode(CodeEmitter* emitter, size_t baseAddress,
        ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const;

    ProgramSection* clone() const;

private:
    std::string mName;
    std::vector<Instruction*> mInstructions;
    mutable std::optional<size_t> mCalculatedSize;

    DISABLE_COPY(ProgramSection);
};

#endif
