#ifndef COMPILER_ASSEMBLER_MACROREPEAT_H
#define COMPILER_ASSEMBLER_MACROREPEAT_H

#include "Compiler/Tree/Value.h"
#include "Compiler/Assembler/Instruction.h"

class Expr;

class MacroRepeat final : public Instruction
{
public:
    MacroRepeat(SourceLocation* location, Expr* count)
        : Instruction(location)
        , mCount(count)
    {
        registerFinalizer();
    }

    Type type() const final override;
    Value& value() const { return mValue; }

    void addInstruction(Instruction* instruction);

    bool resolveLabels(size_t& address,
        ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError);
    void unresolveLabels();

    bool calculateSizeInBytes(size_t& outSize,
        ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const final override;
    bool canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const final override;
    bool emitCode(CodeEmitter* emitter, int64_t& nextAddress, ISectionResolver* sectionResolver,
        std::unique_ptr<CompilerError>& resolveError) const final override;

    void resetCounters() const final override;
    void saveReadCounter() const final override;
    void restoreReadCounter() const final override;
    void advanceCounters() const final override;

    Instruction* clone() const override;

private:
    std::vector<Instruction*> mInstructions;
    Expr* mCount;
    mutable Value mValue;

    DISABLE_COPY(MacroRepeat);
};

#endif
