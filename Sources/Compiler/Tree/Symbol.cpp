#include "Symbol.h"
#include "Compiler/CompilerError.h"
#include "Compiler/Assembler/Label.h"
#include "Compiler/Tree/Expr.h"
#include <sstream>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Symbol::Type ConstantSymbol::type() const
{
    return Constant;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Symbol::Type ConditionalConstantSymbol::type() const
{
    return ConditionalConstant;
}

void ConditionalConstantSymbol::addValue(Expr* condition, Expr* value)
{
    Entry entry;
    entry.condition = condition;
    entry.value = value;
    mEntries.emplace_back(entry);
}

bool ConditionalConstantSymbol::canEvaluateValue(const int64_t* currentAddress,
    std::unique_ptr<CompilerError>& resolveError) const
{
    for (const auto& it : mEntries) {
        if (!it.condition->canEvaluateValue(currentAddress, resolveError))
            return false;
        if (!it.value->canEvaluateValue(currentAddress, resolveError))
            return false;
    }
    return true;
}

Expr* ConditionalConstantSymbol::expr(SourceLocation* location, const int64_t* currentAddress) const
{
    Expr* value = nullptr;
    for (const auto& it : mEntries) {
        bool isThis = it.condition->evaluateValue(currentAddress).number != 0;
        if (isThis) {
            if (value) {
                std::stringstream ss;
                ss << "conflicting value for symbol \"" << name() << "\".";
                throw CompilerError(it.condition->location(), ss.str());
            }
            value = it.value;
        }
    }

    if (!value) {
        std::stringstream ss;
        ss << "unable to resolve symbol \"" << name() << "\".";
        throw CompilerError(location, ss.str());
    }

    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LabelSymbol::LabelSymbol(SourceLocation* location, ::Label* label)
    : Symbol(location, label->name())
    , mLabel(label)
{
}

Symbol::Type LabelSymbol::type() const
{
    return Label;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Symbol::Type ConditionalLabelSymbol::type() const
{
    return ConditionalLabel;
}

void ConditionalLabelSymbol::addLabel(Expr* condition, ::Label* label)
{
    Entry entry;
    entry.condition = condition;
    entry.label = label;
    mEntries.emplace_back(entry);
}

bool ConditionalLabelSymbol::canEvaluateValue(const int64_t* currentAddress,
    std::unique_ptr<CompilerError>& resolveError) const
{
    for (const auto& it : mEntries) {
        if (!it.condition->canEvaluateValue(currentAddress, resolveError))
            return false;
    }

    for (const auto& it : mEntries) {
        bool isThis = it.condition->evaluateValue(currentAddress).number != 0;
        if (isThis && !it.label->hasAddress())
            return false;
    }

    return true;
}

Label* ConditionalLabelSymbol::label(SourceLocation* location, const int64_t* currentAddress) const
{
    ::Label* label = nullptr;
    for (const auto& it : mEntries) {
        bool isThis = it.condition->evaluateValue(currentAddress).number != 0;
        if (isThis) {
            if (label) {
                std::stringstream ss;
                ss << "conflicting label \"" << name() << "\".";
                throw CompilerError(it.condition->location(), ss.str());
            }
            label = it.label;
        }
    }

    if (!label) {
        std::stringstream ss;
        ss << "unable to resolve label \"" << name() << "\".";
        throw CompilerError(location, ss.str());
    }

    return label;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RepeatVariableSymbol::RepeatVariableSymbol(SourceLocation* location, const char* name, Value* value)
    : Symbol(location, name)
    , mValue(value)
{
}

Symbol::Type RepeatVariableSymbol::type() const
{
    return RepeatVariable;
}
