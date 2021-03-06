#include "AssemblerContextIf.h"
#include "Compiler/Assembler/MacroIf.h"
#include "Compiler/Linker/ProgramSection.h"
#include "Compiler/Token.h"
#include "Compiler/CompilerError.h"

AssemblerContextIf::AssemblerContextIf(AssemblerContext* prev, const Token* token, Expr* condition)
    : AssemblerContext(prev)
    , mCondition(condition)
    , mIfToken(token)
    , mHasElse(false)
{
    AssemblerContext::setCurrentSection(prev->currentSection());

    if (!currentSection())
        throw CompilerError(token->location(), "code or data not in a section.");

    mMacro = new (heap()) MacroIf(token->location(), condition);
    prev->addInstruction(mMacro);
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

const std::string& AssemblerContextIf::localLabelsPrefix() const
{
    return prev()->localLabelsPrefix();
}

void AssemblerContextIf::setLocalLabelsPrefix(SourceLocation* location, std::string)
{
    throw CompilerError(location, "global labels are not allowed at this point.");
}

bool AssemblerContextIf::setCurrentSection(ProgramSection*)
{
    return false;
}

void AssemblerContextIf::addInstruction(Instruction* instruction)
{
    if (mHasElse)
        mMacro->addElseInstruction(instruction);
    else
        mMacro->addThenInstruction(instruction);
}
