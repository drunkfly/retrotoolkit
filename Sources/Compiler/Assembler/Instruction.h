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

    bool resolveLabel(size_t& address, ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError);
    void unresolveLabel();

    virtual bool calculateSizeInBytes(size_t& outSize,
        ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const = 0;
    virtual bool canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const = 0;
    virtual bool emitCode(CodeEmitter* emitter, int64_t& nextAddress, ISectionResolver* sectionResolver,
        std::unique_ptr<CompilerError>& resolveError) const = 0;

    virtual Instruction* clone() const = 0;

    virtual void resetCounters() const;
    virtual void saveReadCounter() const;
    virtual void restoreReadCounter() const;
    virtual void advanceCounters() const;

    static void copyInstructions(std::vector<Instruction*>& target, const std::vector<Instruction*>& source);

private:
    SourceLocation* mLocation;

    DISABLE_COPY(Instruction);
};

#endif
