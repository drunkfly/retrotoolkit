#include "MacroIf.h"
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

bool MacroIf::resolveLabels(size_t& address, std::unique_ptr<CompilerError>& resolveError)
{
    if (!mCondition->canEvaluateValue(nullptr, resolveError))
        return false;

    auto result = mCondition->evaluateValue(nullptr).number;
    const std::vector<Instruction*>& instructions = (result ? mThenInstructions : mElseInstructions);

    for (const auto& instruction : instructions) {
        if (!instruction->resolveLabel(address, resolveError))
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

bool MacroIf::calculateSizeInBytes(size_t& outSize, std::unique_ptr<CompilerError>& resolveError) const
{
    if (!mCondition->canEvaluateValue(nullptr, resolveError))
        return false;

    auto result = mCondition->evaluateValue(nullptr).number;
    const std::vector<Instruction*>& instructions = (result ? mThenInstructions : mElseInstructions);

    outSize = 0;
    for (const auto& instruction : instructions) {
        size_t size;
        if (!instruction->calculateSizeInBytes(size, resolveError))
            return false;
        outSize += size;
    }

    return true;
}

bool MacroIf::emitCode(CodeEmitter* emitter, int64_t& nextAddress, std::unique_ptr<CompilerError>& resolveError) const
{
    if (!mCondition->canEvaluateValue(nullptr, resolveError))
        return false;

    auto result = mCondition->evaluateValue(nullptr).number;
    const std::vector<Instruction*>& instructions = (result ? mThenInstructions : mElseInstructions);

    for (const auto& instruction : instructions) {
        if (!instruction->emitCode(emitter, nextAddress, resolveError))
            return false;
    }

    return true;
}
