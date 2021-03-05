#include "AssemblerContextRepeat.h"

AssemblerContextRepeat::AssemblerContextRepeat(AssemblerContext* prev, const Token* token, std::string var, Expr* count)
    : AssemblerContext(prev)
    , mVariable(std::move(var))
    //, mCodeEmitter(compiler->allocCodeEmitter<RepeatedCodeEmitter>(std::move(count)))
    , mToken(token)
{
    AssemblerContext::setCurrentSection(prev->currentSection());
}

bool AssemblerContextRepeat::isRepeat() const
{
    return true;
}

bool AssemblerContextRepeat::hasVariable(const std::string& name) const
{
    return mVariable == name || AssemblerContext::hasVariable(name);
}

bool AssemblerContextRepeat::setCurrentSection(ProgramSection*)
{
    return false;
}
