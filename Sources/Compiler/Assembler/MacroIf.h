#ifndef COMPILER_ASSEMBLER_MACROIF_H
#define COMPILER_ASSEMBLER_MACROIF_H

#include "Compiler/Assembler/Instruction.h"

class Expr;

class MacroIf final : public Instruction
{
public:
    explicit MacroIf(SourceLocation* location, Expr* condition)
        : Instruction(location)
        , mCondition(condition)
    {
        registerFinalizer();
    }

    Type type() const final override;

    void addThenInstruction(Instruction* instruction);
    void addElseInstruction(Instruction* instruction);

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
    Expr* mCondition;
    std::vector<Instruction*> mThenInstructions;
    std::vector<Instruction*> mElseInstructions;

    DISABLE_COPY(MacroIf);
};

#endif
