#include "ProgramSection.h"
#include "Compiler/CompilerError.h"
#include "Compiler/Linker/CodeEmitter.h"
#include "Compiler/Assembler/Instruction.h"
#include "Compiler/Assembler/Label.h"

ProgramSection::ProgramSection(std::string name)
    : mName(std::move(name))
{
    registerFinalizer();
}

ProgramSection::~ProgramSection()
{
}

size_t ProgramSection::calculateSizeInBytes() const
{
    size_t size = 0;
    for (const auto& instruction : mInstructions)
        size += instruction->sizeInBytes();
    return size;
}

void ProgramSection::resolveLabels(size_t address)
{
    for (const auto& instruction : mInstructions) {
        if (address > 0xffff)
            throw CompilerError(instruction->location(), "address is over 64K.");

        if (instruction->isLabel())
            static_cast<Label*>(instruction)->setAddress(address);
        else
            address += instruction->sizeInBytes();
    }
}

void ProgramSection::unresolveLabels()
{
    for (const auto& instruction : mInstructions) {
        if (instruction->isLabel())
            static_cast<Label*>(instruction)->unsetAddress();
    }
}

void ProgramSection::addInstruction(Instruction* instruction)
{
    mInstructions.emplace_back(instruction);
}

bool ProgramSection::emitCode(CodeEmitter* emitter, size_t baseAddress,
    std::unique_ptr<CompilerError>& resolveError) const
{
    int64_t nextAddress = int64_t(baseAddress);
    for (const auto& instruction : mInstructions) {
        if (nextAddress > 0xffff) {
            resolveError = std::make_unique<CompilerError>(instruction->location(), "address is over 64K.");
            return false;
        }

        if (!instruction->emitCode(emitter, nextAddress, resolveError))
            return false;

        if (nextAddress > 0x10000) {
            resolveError = std::make_unique<CompilerError>(instruction->location(), "address is over 64K.");
            return false;
        }
    }

    return true;
}
