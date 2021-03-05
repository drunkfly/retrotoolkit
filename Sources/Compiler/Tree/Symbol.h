#ifndef COMPILER_TREE_SYMBOL_H
#define COMPILER_TREE_SYMBOL_H

#include "Common/GC.h"
#include <string>

class Expr;
class SourceLocation;
class Label;
class Value;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Symbol : public GCObject
{
public:
    enum Type
    {
        Constant,
        Label,
        RepeatVariable,
    };

    Symbol(SourceLocation* location, const char* name)
        : mLocation(location)
        , mName(name)
    {
    }

    SourceLocation* location() const { return mLocation; }
    const char* name() const { return mName; }

    virtual Type type() const = 0;

private:
    SourceLocation* mLocation;
    const char* mName;

    DISABLE_COPY(Symbol);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ConstantSymbol : public Symbol
{
public:
    ConstantSymbol(SourceLocation* location, const char* name, Expr* value)
        : Symbol(location, name)
        , mValue(value)
    {
    }

    Type type() const final override;

    const Expr* value() const { return mValue; }

private:
    Expr* mValue;

    DISABLE_COPY(ConstantSymbol);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class LabelSymbol : public Symbol
{
public:
    LabelSymbol(SourceLocation* location, ::Label* label);

    Type type() const final override;

    ::Label* label() const { return mLabel; }

private:
    ::Label* mLabel;

    DISABLE_COPY(LabelSymbol);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RepeatVariableSymbol : public Symbol
{
public:
    RepeatVariableSymbol(SourceLocation* location, const char* name, Value* value);

    Type type() const final override;

    Value* value() const { return mValue; }

private:
    Value* mValue;

    DISABLE_COPY(RepeatVariableSymbol);
};

#endif
