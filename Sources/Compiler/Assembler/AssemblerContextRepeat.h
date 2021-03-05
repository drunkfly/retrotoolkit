#ifndef COMPILER_ASSEMBLER_ASSEMBLERCONTEXTREPEAT_H
#define COMPILER_ASSEMBLER_ASSEMBLERCONTEXTREPEAT_H

#include "Compiler/Assembler/AssemblerContext.h"

class Expr;

class AssemblerContextRepeat final : public AssemblerContext
{
public:
    AssemblerContextRepeat(AssemblerContext* prev, const Token* token, std::string var, Expr* count);

    /*
    std::unique_ptr<AssemblerContext> clone() const override;
    */

    bool isRepeat() const final override;
    bool hasVariable(const std::string& name) const override;

    /*
    std::string localLabelsPrefix() const override;
    void setLocalLabelsPrefix(std::string prefix, const Token& token, IErrorReporter* reporter) override;
    bool areGlobalLabelsAllowed() const override;
    void adjustLabel(ProgramLabel* label) override;
    */

    bool setCurrentSection(ProgramSection* section) override;

    /*
    CodeEmitter* codeEmitter() const override;
    std::shared_ptr<RepeatedCodeEmitter> codeEmitterSharedPtr() const { return mCodeEmitter.lock(); }
    */

private:
    std::string mVariable;
    //std::weak_ptr<RepeatedCodeEmitter> mCodeEmitter;
    const Token* mToken;

    DISABLE_COPY(AssemblerContextRepeat);
};

#endif
