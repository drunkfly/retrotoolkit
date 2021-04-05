#ifndef COMPILER_ASSEMBLER_INSTRUCTION_H
#define COMPILER_ASSEMBLER_INSTRUCTION_H

#include "Common/GC.h"
#include "Compiler/Tree/SourceLocation.h"

class ISectionResolver;
class CodeEmitter;
class CompilerError;

class Instruction : public GCObject
{
public:
    enum class Type
    {
        Default,
        Label,
        If,
        Repeat,
    };

    explicit Instruction(SourceLocation* location)
        : mLocation(location)
    {
    }

    virtual Type type() const;
    virtual bool isZ80Opcode() const;

    SourceLocation* location() const { return mLocation; }

    bool resolveLabel(size_t& address, std::unique_ptr<CompilerError>& resolveError);
    void unresolveLabel();

    virtual bool calculateSizeInBytes(size_t& outSize, std::unique_ptr<CompilerError>& resolveError) const = 0;
    virtual bool canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const = 0;
    virtual bool emitCode(CodeEmitter* emitter, int64_t& nextAddress, ISectionResolver* sectionResolver,
        std::unique_ptr<CompilerError>& resolveError) const = 0;

private:
    SourceLocation* mLocation;

    DISABLE_COPY(Instruction);
};

#endif
