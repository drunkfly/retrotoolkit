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

bool MacroRepeat::emitCode(CodeEmitter* emitter, int64_t& nextAddress,
    std::unique_ptr<CompilerError>& resolveError) const
{
    // FIXME
    return false;
}
