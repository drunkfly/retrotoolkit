#ifndef COMPILER_ASSEMBLER_ASSEMBLERCONTEXTIF_H
#define COMPILER_ASSEMBLER_ASSEMBLERCONTEXTIF_H

#include "Compiler/Assembler/AssemblerContext.h"

class MacroIf;
class Expr;

class AssemblerContextIf final : public AssemblerContext
{
public:
    AssemblerContextIf(AssemblerContext* prev, const Token* token, Expr* condition);

    bool isIf() const final override;
    bool hasElse() const final override;
    void beginElse(const Token* token) final override;

    const std::string& localLabelsPrefix() const override;
    void setLocalLabelsPrefix(SourceLocation* location, std::string prefix) override;

    bool setCurrentSection(ProgramSection* section) override;

    void addInstruction(Instruction* instruction) override;

private:
    MacroIf* mMacro;
    Expr* mCondition;
    const Token* mIfToken;
    const Token* mElseToken;
    bool mHasElse;

    DISABLE_COPY(AssemblerContextIf);
};

#endif
