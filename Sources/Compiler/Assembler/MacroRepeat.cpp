#include "MacroRepeat.h"
#include "Compiler/CompilerError.h"
#include "Compiler/Tree/Expr.h"

Instruction::Type MacroRepeat::type() const
{
    return Type::Repeat;
}

void MacroRepeat::addInstruction(Instruction* instruction)
{
    mInstructions.emplace_back(instruction);
}

bool MacroRepeat::resolveLabels(size_t& address,
    ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError)
{
    if (!mCount->canEvaluateValue(nullptr, sectionResolver, resolveError))
        return false;

    auto count = mCount->evaluateValue(nullptr, sectionResolver).number;
    if (count <= 0)
        return true;

    for (const auto& instruction : mInstructions)
        instruction->saveReadCounter();

    for (uint64_t i = 0; i < count; i++) {
        mValue = Value(int64_t(i));
        for (const auto& instruction : mInstructions) {
            if (!instruction->resolveLabel(address, sectionResolver, resolveError))
                return false;
        }

        for (const auto& instruction : mInstructions)
            instruction->advanceCounters();
    }

    mValue = Value(0);
    for (const auto& instruction : mInstructions)
        instruction->restoreReadCounter();

    return true;
}

void MacroRepeat::unresolveLabels()
{
    for (const auto& instruction : mInstructions)
        instruction->unresolveLabel();
}

bool MacroRepeat::calculateSizeInBytes(size_t& outSize,
    ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const
{
    outSize = 0;

    if (!mCount->canEvaluateValue(nullptr, sectionResolver, resolveError))
        return false;

    auto count = mCount->evaluateValue(nullptr, sectionResolver).number;
    if (count <= 0)
        return true;

    for (const auto& instruction : mInstructions)
        instruction->saveReadCounter();

    for (uint64_t i = 0; i < count; i++) {
        mValue = Value(int64_t(i));
        for (const auto& instruction : mInstructions) {
            size_t size = 0;
            if (!instruction->calculateSizeInBytes(size, sectionResolver, resolveError))
                return false;
            outSize += size;
        }

        for (const auto& instruction : mInstructions)
            instruction->advanceCounters();
    }

    mValue = Value(0);
    for (const auto& instruction : mInstructions)
        instruction->restoreReadCounter();

    return true;
}

bool MacroRepeat::canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const
{
    std::unique_ptr<CompilerError> resolveError;
    if (!mCount->canEvaluateValue(nullptr, sectionResolver, resolveError))
        return false;

    auto count = mCount->evaluateValue(nullptr, sectionResolver).number;
    if (count <= 0)
        return true;

    for (const auto& instruction : mInstructions) {
        if (!instruction->canEmitCodeWithoutBaseAddress(sectionResolver))
            return false;
    }

    return true;
}

bool MacroRepeat::emitCode(CodeEmitter* emitter, int64_t& nextAddress, ISectionResolver* sectionResolver,
    std::unique_ptr<CompilerError>& resolveError) const
{
    auto count = mCount->evaluateValue(nullptr, nullptr).number;
    if (count < 0)
        throw CompilerError(location(), "repeat counter is negative.");
    if (count > 0xffff)
        throw CompilerError(location(), "repeat counter is too large.");
    if (count == 0)
        return true;

    for (const auto& instruction : mInstructions)
        instruction->saveReadCounter();

    for (uint64_t i = 0; i < count; i++) {
        mValue = Value(int64_t(i));
        for (const auto& instruction : mInstructions) {
            if (!instruction->emitCode(emitter, nextAddress, sectionResolver, resolveError))
                return false;
        }

        for (const auto& instruction : mInstructions)
            instruction->advanceCounters();
    }

    mValue = Value(0);
    for (const auto& instruction : mInstructions)
        instruction->restoreReadCounter();

    return true;
}

void MacroRepeat::resetCounters() const
{
    for (const auto& instruction : mInstructions)
        instruction->resetCounters();
}

void MacroRepeat::saveReadCounter() const
{
}

void MacroRepeat::restoreReadCounter() const
{
}

void MacroRepeat::advanceCounters() const
{
}

Instruction* MacroRepeat::clone() const
{
    MacroRepeat* copy = new (heap()) MacroRepeat(location(), mCount);
    copyInstructions(copy->mInstructions, mInstructions);
    return copy;
}
