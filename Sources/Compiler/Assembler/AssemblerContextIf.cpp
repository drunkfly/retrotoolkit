#include "AssemblerContextIf.h"

AssemblerContextIf::AssemblerContextIf(AssemblerContext* prev, const Token* token)
    : AssemblerContext(prev)
    //, mThenCodeEmitter(compiler->allocCodeEmitter<CodeEmitter>())
    //, mElseCodeEmitter(compiler->allocCodeEmitter<CodeEmitter>())
    //, mIfToken(token)
    //, mHasElse(false)
{
    AssemblerContext::setCurrentSection(prev->currentSection());
}

bool AssemblerContextIf::isIf() const final override
{
    return true;
}

bool AssemblerContextIf::setCurrentSection(ProgramSection*)
{
    return false;
}
