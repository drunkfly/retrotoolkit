#ifndef COMPILER_ASSEMBLER_MACROENSURE_H
#define COMPILER_ASSEMBLER_MACROENSURE_H

#include "Compiler/Assembler/Instruction.h"

class Expr;

class MacroEnsure final : public Instruction
{
public:
    explicit MacroEnsure(SourceLocation* location, Expr* condition)
        : Instruction(location)
        , mCondition(condition)
    {
    }

    bool calculateSizeInBytes(size_t& outSize,
        ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const final override;
    bool canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const final override;
    bool emitCode(CodeEmitter* emitter, int64_t& nextAddress, ISectionResolver* sectionResolver,
        std::unique_ptr<CompilerError>& resolveError) const final override;

    Instruction* clone() const override;

private:
    Expr* mCondition;

    DISABLE_COPY(MacroEnsure);
};

#endif
