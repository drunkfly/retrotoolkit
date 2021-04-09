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
        if (!instruction->resolveLabel(address, resolveError))
            return false;
    }
    return true;
}

void ProgramSection::unresolveLabels()
{
    for (const auto& instruction : mInstructions)
        instruction->unresolveLabel();
}

void ProgramSection::addInstruction(Instruction* instruction)
{
    mInstructions.emplace_back(instruction);
}

bool ProgramSection::canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const
{
    for (const auto& instruction : mInstructions) {
        if (!instruction->canEmitCodeWithoutBaseAddress(sectionResolver))
            return false;
    }

    return true;
}

bool ProgramSection::emitCode(CodeEmitter* emitter, size_t baseAddress, ISectionResolver* sectionResolver,
    std::unique_ptr<CompilerError>& resolveError) const
{
    int64_t nextAddress = int64_t(baseAddress);
    int64_t startAddress = nextAddress;

    for (const auto& instruction : mInstructions) {
        if (nextAddress > 0xffff) {
            resolveError = std::make_unique<CompilerError>(instruction->location(), "address is over 64K.");
            return false;
        }

        if (!instruction->emitCode(emitter, nextAddress, sectionResolver, resolveError))
            return false;

        if (nextAddress > 0x10000) {
            resolveError = std::make_unique<CompilerError>(instruction->location(), "address is over 64K.");
            return false;
        }
    }

    if (mCalculatedSize && *mCalculatedSize != nextAddress - startAddress) {
        SourceLocation* location = (!mInstructions.empty() ? mInstructions.back()->location() : nullptr);
        std::stringstream ss;
        ss << "internal compiler error: mismatch of calculated and generated size for section \"" << mName << "\".";
        resolveError = std::make_unique<CompilerError>(location, ss.str());
        return false;
    }

    emitter->addSectionDebugInfo(mName, startAddress, Compression::None, (nextAddress - startAddress), {});
    return true;
}

ProgramSection* ProgramSection::clone() const
{
    ProgramSection* copy = new (heap()) ProgramSection(mName);
    Instruction::copyInstructions(copy->mInstructions, mInstructions);
    return copy;
}
