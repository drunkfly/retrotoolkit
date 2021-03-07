#include "AssemblerContextRepeat.h"
#include "Compiler/Assembler/MacroRepeat.h"
#include "Compiler/Linker/ProgramSection.h"
#include "Compiler/Token.h"
#include "Compiler/CompilerError.h"

AssemblerContextRepeat::AssemblerContextRepeat(AssemblerContext* prev, const Token* token, std::string var, Expr* count)
    : AssemblerContext(prev)
    , mVariable(std::move(var))
    , mCount(count)
    , mToken(token)
{
    registerFinalizer();
    AssemblerContext::setCurrentSection(prev->currentSection());

    if (!currentSection())
        throw CompilerError(token->location(), "code or data not in a section.");

    mMacro = new (heap()) MacroRepeat(token->location());
    prev->addInstruction(mMacro);
}

bool AssemblerContextRepeat::isRepeat() const
{
    return true;
}

bool AssemblerContextRepeat::hasVariable(const std::string& name) const
{
    return mVariable == name || AssemblerContext::hasVariable(name);
}

const std::string& AssemblerContextRepeat::localLabelsPrefix() const
{
    return prev()->localLabelsPrefix();
}

void AssemblerContextRepeat::setLocalLabelsPrefix(SourceLocation* location, std::string prefix)
{
    prev()->setLocalLabelsPrefix(location, prefix);
}

bool AssemblerContextRepeat::setCurrentSection(ProgramSection*)
{
    return false;
}

void AssemblerContextRepeat::addInstruction(Instruction* instruction)
{
    mMacro->addInstruction(instruction);
}
