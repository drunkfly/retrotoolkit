#include "MacroIf.h"
#include "Compiler/CompilerError.h"
#include "Compiler/Tree/Expr.h"

Instruction::Type MacroIf::type() const
{
    return Type::If;
}

void MacroIf::addThenInstruction(Instruction* instruction)
{
    mThenInstructions.emplace_back(instruction);
}

void MacroIf::addElseInstruction(Instruction* instruction)
{
    mElseInstructions.emplace_back(instruction);
}

bool MacroIf::resolveLabels(size_t& address,
    ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError)
{
    if (!mCondition->canEvaluateValue(nullptr, sectionResolver, resolveError))
        return false;

    auto result = mCondition->evaluateValue(nullptr, sectionResolver).number;
    const std::vector<Instruction*>& instructions = (result ? mThenInstructions : mElseInstructions);

    for (const auto& instruction : instructions) {
        if (!instruction->resolveLabel(address, sectionResolver, resolveError))
            return false;
    }

    return true;
}

void MacroIf::unresolveLabels()
{
    for (const auto& instruction : mThenInstructions)
        instruction->unresolveLabel();
    for (const auto& instruction : mElseInstructions)
        instruction->unresolveLabel();
}

bool MacroIf::calculateSizeInBytes(size_t& outSize,
    ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const
{
    if (!mCondition->canEvaluateValue(nullptr, sectionResolver, resolveError))
        return false;

    auto result = mCondition->evaluateValue(nullptr, sectionResolver).number;
    const std::vector<Instruction*>& instructions = (result ? mThenInstructions : mElseInstructions);

    outSize = 0;
    for (const auto& instruction : instructions) {
        size_t size;
        if (!instruction->calculateSizeInBytes(size, sectionResolver, resolveError))
            return false;
        outSize += size;
    }

    return true;
}

bool MacroIf::canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const
{
    std::unique_ptr<CompilerError> resolveError;
    if (!mCondition->canEvaluateValue(nullptr, sectionResolver, resolveError))
        return false;

    auto result = mCondition->evaluateValue(nullptr, sectionResolver).number;
    const std::vector<Instruction*>& instructions = (result ? mThenInstructions : mElseInstructions);

    for (const auto& instruction : instructions) {
        if (!instruction->canEmitCodeWithoutBaseAddress(sectionResolver))
            return false;
    }

    return true;
}

bool MacroIf::emitCode(CodeEmitter* emitter,
    int64_t& nextAddress, ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const
{
    if (!mCondition->canEvaluateValue(nullptr, nullptr, resolveError))
        return false;

    auto result = mCondition->evaluateValue(nullptr, nullptr).number;
    const std::vector<Instruction*>& instructions = (result ? mThenInstructions : mElseInstructions);

    for (const auto& instruction : instructions) {
        if (!instruction->emitCode(emitter, nextAddress, sectionResolver, resolveError))
            return false;
    }

    return true;
}

Instruction* MacroIf::clone() const
{
    MacroIf* copy = new (heap()) MacroIf(location(), mCondition);
    copyInstructions(copy->mThenInstructions, mThenInstructions);
    copyInstructions(copy->mElseInstructions, mElseInstructions);
    return copy;
}
