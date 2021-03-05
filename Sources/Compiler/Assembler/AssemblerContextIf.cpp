#include "AssemblerContextIf.h"
#include "Compiler/Token.h"
#include "Compiler/CompilerError.h"

AssemblerContextIf::AssemblerContextIf(AssemblerContext* prev, const Token* token)
    : AssemblerContext(prev)
    //, mThenCodeEmitter(compiler->allocCodeEmitter<CodeEmitter>())
    //, mElseCodeEmitter(compiler->allocCodeEmitter<CodeEmitter>())
    , mIfToken(token)
    , mHasElse(false)
{
    AssemblerContext::setCurrentSection(prev->currentSection());
}

bool AssemblerContextIf::isIf() const
{
    return true;
}

bool AssemblerContextIf::hasElse() const
{
    return mHasElse;
}

void AssemblerContextIf::beginElse(const Token* token)
{
    if (mHasElse)
        throw CompilerError(token->location(), "internal compiler error: beginElse() called inside else block.");
    mHasElse = true;
    mElseToken = token;
}

bool AssemblerContextIf::setCurrentSection(ProgramSection*)
{
    return false;
}
