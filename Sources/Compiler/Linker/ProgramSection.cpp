#include "ProgramSection.h"
#include "Compiler/CompilerError.h"
#include "Compiler/Linker/CodeEmitter.h"
#include "Compiler/Assembler/Instruction.h"
#include "Compiler/Assembler/Label.h"
#include <sstream>

ProgramSection::ProgramSection(std::string name)
    : mName(std::move(name))
{
    registerFinalizer();
}

ProgramSection::~ProgramSection()
{
}

bool ProgramSection::calculateSizeInBytes(size_t& outSize, std::unique_ptr<CompilerError>& resolveError) const
{
    outSize = 0;
    for (const auto& instruction : mInstructions) {
        size_t size;
        if (!instruction->calculateSizeInBytes(size, resolveError))
            return false;
        outSize += size;
    }
    mCalculatedSize = outSize;
    return true;
}

bool ProgramSection::resolveLabels(size_t address, std::unique_ptr<CompilerError>& resolveError)
{
    for (const auto& instruction : mInstructions) {
        if (address > 0xffff)
            throw CompilerError(instruction->location(), "address is over 64K.");

        if (instruction->isLabel())
            static_cast<Label*>(instruction)->setAddress(address);
        else {
            size_t size;
            if (!instruction->calculateSizeInBytes(size, resolveError))
                return false;
            address += size;
        }
    }
    return true;
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

    if (mCalculatedSize && *mCalculatedSize != nextAddress - baseAddress) {
        SourceLocation* location = (!mInstructions.empty() ? mInstructions.back()->location() : nullptr);
        std::stringstream ss;
        ss << "internal compiler error: mismatch of calculated and generated size for section \"" << mName << "\".";
        resolveError = std::make_unique<CompilerError>(location, ss.str());
        return false;
    }

    return true;
}
