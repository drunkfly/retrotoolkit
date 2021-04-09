#include "MacroRepeat.h"

Instruction::Type MacroRepeat::type() const
{
    return Type::Repeat;
}

void MacroRepeat::addInstruction(Instruction* instruction)
{
    mInstructions.emplace_back(instruction);
}

bool MacroRepeat::resolveLabels(size_t& address, std::unique_ptr<CompilerError>& resolveError)
{
    // FIXME
    return false;
}

void MacroRepeat::unresolveLabels()
{
    // FIXME
}

bool MacroRepeat::calculateSizeInBytes(size_t& outSize, std::unique_ptr<CompilerError>& resolveError) const
{
    // FIXME
    return false;
}

bool MacroRepeat::canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const
{
    // FIXME
    return true;
}

bool MacroRepeat::emitCode(CodeEmitter* emitter, int64_t& nextAddress, ISectionResolver* sectionResolver,
    std::unique_ptr<CompilerError>& resolveError) const
{
    // FIXME
    return false;
}

Instruction* MacroRepeat::clone() const
{
    MacroRepeat* copy = new (heap()) MacroRepeat(location());
    copyInstructions(copy->mInstructions, mInstructions);
    return copy;
}
