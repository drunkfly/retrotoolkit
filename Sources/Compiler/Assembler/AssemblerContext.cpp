#include "AssemblerContext.h"
#include "Compiler/Assembler/Instruction.h"
#include "Compiler/Assembler/Label.h"
#include "Compiler/Linker/ProgramSection.h"
#include "Compiler/Tree/Symbol.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/Token.h"
#include "Compiler/CompilerError.h"
#include <sstream>

AssemblerContext::AssemblerContext(AssemblerContext* prev)
    : mPrev(prev)
    , mSection(nullptr)
    , mEphemeralLabelCounter(0)
{
    registerFinalizer();
}

AssemblerContext::~AssemblerContext()
{
}

bool AssemblerContext::isIf() const
{
    return false;
}

bool AssemblerContext::hasElse() const
{
    throw CompilerError(nullptr, "internal compiler error: hasElse() called on invalid context.");
}

void AssemblerContext::beginElse(Token* token)
{
    throw CompilerError(token->location(), "internal compiler error: beginElse() called on invalid context.");
}

bool AssemblerContext::isRepeat() const
{
    return false;
}

bool AssemblerContext::hasVariable(const std::string& name) const
{
    return (mPrev ? mPrev->hasVariable(name) : false);
}

const std::string& AssemblerContext::localLabelsPrefix() const
{
    return mLocalLabelsPrefix;
}

void AssemblerContext::setLocalLabelsPrefix(SourceLocation* location, std::string prefix)
{
    if (prefix.find("@@") != std::string::npos) {
        assert(false);
        throw CompilerError(location, "internal compiler error: local labels prefix contains '@@'.");
    }
    mLocalLabelsPrefix = std::move(prefix);
}

bool AssemblerContext::setCurrentSection(ProgramSection* section)
{
    mSection = section;
    return true;
}

void AssemblerContext::addInstruction(Instruction* instruction)
{
    if (!mSection)
        throw CompilerError(instruction->location(), "code or data not in a section.");
    mSection->addInstruction(instruction);
}

void AssemblerContext::addLabel(SymbolTable* symbolTable, SourceLocation* location, std::string name)
{
    if (!mSection)
        throw CompilerError(location, "label not in a section.");
    auto label = new (heap()) Label(location, std::move(name));
    if (!symbolTable->addSymbol(new (heap()) LabelSymbol(location, label))) {
        std::stringstream ss;
        ss << "duplicate identifier \"" << label->name() << "\".";
        throw CompilerError(location, ss.str());
    }
    mSection->addInstruction(label);
}

Label* AssemblerContext::addEphemeralLabel(SourceLocation* location)
{
    if (!mSection)
        throw CompilerError(location, "current address is not available outside of a section.");

    std::stringstream ss;
    ss << '$' << mEphemeralLabelCounter++;
    auto label = new (heap()) Label(location, ss.str());
    mSection->addInstruction(label);

    return label;
}
