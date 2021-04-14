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

    void validateAtPop(SourceLocation* location) final override;

    const std::string& localLabelsPrefix() const override;
    void setLocalLabelsPrefix(SourceLocation* location, std::string prefix) override;

    Expr* condition() const override;

    bool setCurrentSection(ProgramSection* section) override;

    void addInstruction(Instruction* instruction) override;
    void addConstant(SymbolTable* symbolTable, SourceLocation* location, const char* name, Expr* value) override;
    void addLabel(SymbolTable* symbolTable, SourceLocation* location, const char* name, size_t offset) override;

private:
    MacroIf* mMacro;
    Expr* mCondition;
    const Token* mIfToken;
    const Token* mElseToken;
    SourceLocation* mThenPrefixLocation;
    SourceLocation* mElsePrefixLocation;
    std::string mThenPrefix;
    std::string mElsePrefix;
    bool mHasElse;

    DISABLE_COPY(AssemblerContextIf);
};

#endif
