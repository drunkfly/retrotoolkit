#ifndef COMPILER_TREE_SYMBOL_H
#define COMPILER_TREE_SYMBOL_H

#include "Common/GC.h"

class Expr;
class SourceLocation;
class Label;
class Value;
class CompilerError;
class ISectionResolver;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Symbol : public GCObject
{
public:
    enum Type
    {
        Constant,
        Label,
        ConditionalConstant,
        ConditionalLabel,
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

class ConditionalConstantSymbol : public Symbol
{
public:
    ConditionalConstantSymbol(SourceLocation* location, const char* name)
        : Symbol(location, name)
    {
        registerFinalizer();
    }

    Type type() const final override;

    void addValue(Expr* condition, Expr* value);

    bool canEvaluateValue(const int64_t* currentAddress,
        ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const;
    Expr* expr(SourceLocation* location, const int64_t* currentAddress, ISectionResolver* sectionResolver) const;

private:
    struct Entry
    {
        Expr* condition;
        Expr* value;
    };

    std::vector<Entry> mEntries;

    DISABLE_COPY(ConditionalConstantSymbol);
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

class ConditionalLabelSymbol : public Symbol
{
public:
    ConditionalLabelSymbol(SourceLocation* location, const char* name)
        : Symbol(location, name)
    {
        registerFinalizer();
    }

    Type type() const final override;

    void addLabel(Expr* condition, ::Label* label);

    bool canEvaluateValue(const int64_t* currentAddress,
        ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const;
    ::Label* label(SourceLocation* location, const int64_t* currentAddress, ISectionResolver* sectionResolver) const;

private:
    struct Entry
    {
        Expr* condition;
        ::Label* label;
    };

    std::vector<Entry> mEntries;

    DISABLE_COPY(ConditionalLabelSymbol);
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
