#include "MacroEnsure.h"
#include "Compiler/CompilerError.h"
#include "Compiler/Tree/Expr.h"

bool MacroEnsure::calculateSizeInBytes(size_t& outSize, ISectionResolver*, std::unique_ptr<CompilerError>&) const
{
    outSize = 0;
    return true;
}

bool MacroEnsure::canEmitCodeWithoutBaseAddress(ISectionResolver*) const
{
    return true;
}

bool MacroEnsure::emitCode(CodeEmitter* emitter,
    int64_t& nextAddress, ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const
{
    if (!mCondition->canEvaluateValue(nullptr, sectionResolver, resolveError))
        return false;

    auto result = mCondition->evaluateValue(nullptr, sectionResolver).number;
    if (result == 0) {
        std::stringstream ss;
        ss << "expression is false: ";
        mCondition->toString(ss);
        ss << '.';
        throw CompilerError(location(), ss.str());
    }

    return true;
}

Instruction* MacroEnsure::clone() const
{
    return new (heap()) MacroEnsure(location(), mCondition);
}
