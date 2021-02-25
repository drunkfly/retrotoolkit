#include "AssemblerContext.h"
#include "Compiler/Assembler/Instruction.h"
#include "Compiler/Assembler/Label.h"
#include "Compiler/Linker/ProgramSection.h"
#include "Compiler/Tree/Symbol.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/CompilerError.h"

AssemblerContext::AssemblerContext(AssemblerContext* prev)
    : mPrev(prev)
    , mSection(nullptr)
{
    registerFinalizer();
}

AssemblerContext::~AssemblerContext()
{
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
    symbolTable->addSymbol(new (heap()) LabelSymbol(location, label));
    mSection->addInstruction(label);
}
