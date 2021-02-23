#ifndef COMPILER_TREE_SYMBOL_H
#define COMPILER_TREE_SYMBOL_H

#include "Common/GC.h"
#include <string>

class Expr;
class SourceLocation;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Symbol : public GCObject
{
public:
    Symbol::Symbol(SourceLocation* location, const char* name)
        : mLocation(location)
        , mName(name)
    {
    }

    SourceLocation* location() const { return mLocation; }
    const char* name() const { return mName; }

    virtual bool isConstantSymbol() const;

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

    bool isConstantSymbol() const override;

    const Expr* value() const { return mValue; }

private:
    Expr* mValue;

    DISABLE_COPY(ConstantSymbol);
};

#endif
