#ifndef COMPILER_ASSEMBLER_ASSEMBLERCONTEXTREPEAT_H
#define COMPILER_ASSEMBLER_ASSEMBLERCONTEXTREPEAT_H

#include "Compiler/Assembler/AssemblerContext.h"

class Expr;
class MacroRepeat;

class AssemblerContextRepeat final : public AssemblerContext
{
public:
    AssemblerContextRepeat(AssemblerContext* prev, const Token* token, std::string var, Expr* count);

    bool isRepeat() const final override;
    bool hasVariable(const std::string& name) const override;

    const std::string& localLabelsPrefix() const override;
    void setLocalLabelsPrefix(SourceLocation* location, std::string prefix) override;

    bool setCurrentSection(ProgramSection* section) override;

    void addInstruction(Instruction* instruction) override;

private:
    std::string mVariable;
    MacroRepeat* mMacro;
    Expr* mCount;
    const Token* mToken;

    DISABLE_COPY(AssemblerContextRepeat);
};

#endif
