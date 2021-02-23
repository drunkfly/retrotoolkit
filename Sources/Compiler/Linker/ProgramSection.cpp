#include "ProgramSection.h"
#include "Compiler/Assembler/Instruction.h"

ProgramSection::ProgramSection(std::string name)
    : mName(std::move(name))
{
    registerFinalizer();
}

ProgramSection::~ProgramSection()
{
}

void ProgramSection::addInstruction(Instruction* instruction)
{
    mInstructions.emplace_back(instruction);
}
