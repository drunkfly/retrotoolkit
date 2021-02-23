#include "Program.h"
#include "Compiler/Linker/ProgramSection.h"
#include "Compiler/Tree/SymbolTable.h"

Program::Program()
{
    registerFinalizer();
    mGlobals = new (heap()) SymbolTable(nullptr);
}

Program::~Program()
{
}

ProgramSection* Program::getSection(const std::string& name) const
{
    auto it = mSections.find(name);
    return (it != mSections.end() ? it->second : nullptr);
}

ProgramSection* Program::getOrAddSection(const std::string& name)
{
    auto it = mSections.find(name);
    if (it != mSections.end())
        return it->second;

    auto section = new (heap()) ProgramSection(name);
    mSections[name] = section;

    return section;
}
