#include "Symbol.h"
#include "Compiler/Assembler/Label.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Symbol::Type ConstantSymbol::type() const
{
    return Constant;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LabelSymbol::LabelSymbol(SourceLocation* location, ::Label* label)
    : Symbol(location, label->name().c_str())
    , mLabel(label)
{
}

Symbol::Type LabelSymbol::type() const
{
    return Label;
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
