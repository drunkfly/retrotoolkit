#ifndef COMPILER_ASSEMBLER_ASSEMBLERCONTEXTIF_H
#define COMPILER_ASSEMBLER_ASSEMBLERCONTEXTIF_H

#include "Compiler/Assembler/AssemblerContext.h"

class AssemblerContextIf final : public AssemblerContext
{
public:
    AssemblerContextIf(AssemblerContext* prev, const Token* token);

    /*
    std::unique_ptr<AssemblerContext> clone() const override;
    */

    bool isIf() const final override;
    bool hasElse() const final override;
    void beginElse(const Token* token) final override;

    /*
    std::string localLabelsPrefix() const override;
    void setLocalLabelsPrefix(std::string prefix, const Token& token, IErrorReporter* reporter) override;
    bool areGlobalLabelsAllowed() const override;
    */

    bool setCurrentSection(ProgramSection* section) override;

    /*
    CodeEmitter* codeEmitter() const override;
    std::shared_ptr<CodeEmitter> thenCodeEmitter() const { return mThenCodeEmitter.lock(); }
    std::shared_ptr<CodeEmitter> elseCodeEmitter() const { return mElseCodeEmitter.lock(); }
    */

private:
    //std::weak_ptr<CodeEmitter> mThenCodeEmitter;
    //std::weak_ptr<CodeEmitter> mElseCodeEmitter;
    const Token* mIfToken;
    const Token* mElseToken;
    bool mHasElse;

    DISABLE_COPY(AssemblerContextIf);
};

#endif
