#ifndef COMPILER_TREE_EXPR_H
#define COMPILER_TREE_EXPR_H

#include "Common/Common.h"
#include "Common/GC.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/Tree/SourceLocation.h"
#include "Compiler/Tree/Value.h"
#include <string>
#include <memory>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Expr : public GCObject
{
public:
    explicit Expr(SourceLocation* location);

    SourceLocation* location() const { return mLocation; }

    uint8_t evaluateByte() const;
    uint8_t evaluateByteOffset(int64_t nextAddress) const;
    uint16_t evaluateWord() const;
    uint32_t evaluateDWord() const;
    Value evaluateValue() const;

protected:
    virtual Value evaluate() const = 0;

    template <bool SUB, typename T> static Value smartEvaluate(T&& operatr, Value a, Value b);

private:
    SourceLocation* mLocation;
    mutable bool mEvaluating;

    DISABLE_COPY(Expr);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ExprNumber : public Expr
{
public:
    ExprNumber(SourceLocation* location, int64_t value)
        : Expr(location)
        , mValue(value)
    {
    }

private:
    int64_t mValue;

    Value evaluate() const override;

    DISABLE_COPY(ExprNumber);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ExprIdentifier : public Expr
{
public:
    ExprIdentifier(SourceLocation* location, SymbolTable* table, std::string name)
        : Expr(location)
        , mSymbolTable(table)
        , mName(std::move(name))
    {
        registerFinalizer();
    }

private:
    SymbolTable* mSymbolTable;
    std::string mName;

    Value evaluate() const override;

    DISABLE_COPY(ExprIdentifier);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ExprConditional : public Expr
{
public:
    ExprConditional(SourceLocation* location, Expr* cond, Expr* opThen, Expr* opElse)
        : Expr(location)
        , mCondition(cond)
        , mThen(opThen)
        , mElse(opElse)
    {
    }

private:
    Expr* mCondition;
    Expr* mThen;
    Expr* mElse;

    Value evaluate() const override;

    DISABLE_COPY(ExprConditional);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define UNARY_OPERATOR(NAME) \
    class Expr##NAME : public Expr \
    { \
    public: \
        Expr##NAME(SourceLocation* location, Expr* operand) \
            : Expr(location) \
            , mOperand(operand) \
        { \
        } \
    private: \
        Expr* mOperand; \
        Value evaluate() const override; \
        DISABLE_COPY(Expr##NAME); \
    }

#define BINARY_OPERATOR(NAME) \
    class Expr##NAME : public Expr \
    { \
    public: \
        Expr##NAME(SourceLocation* location, Expr* op1, Expr* op2) \
            : Expr(location) \
            , mOperand1(op1) \
            , mOperand2(op2) \
        { \
        } \
    private: \
        Expr* mOperand1; \
        Expr* mOperand2; \
        Value evaluate() const override; \
        DISABLE_COPY(Expr##NAME); \
    }

UNARY_OPERATOR(Negate);
UNARY_OPERATOR(BitwiseNot);
UNARY_OPERATOR(LogicNot);

BINARY_OPERATOR(Add);
BINARY_OPERATOR(Subtract);
BINARY_OPERATOR(Multiply);
BINARY_OPERATOR(Divide);
BINARY_OPERATOR(Modulo);
BINARY_OPERATOR(ShiftLeft);
BINARY_OPERATOR(ShiftRight);
BINARY_OPERATOR(Less);
BINARY_OPERATOR(LessEqual);
BINARY_OPERATOR(Greater);
BINARY_OPERATOR(GreaterEqual);
BINARY_OPERATOR(Equal);
BINARY_OPERATOR(NotEqual);
BINARY_OPERATOR(BitwiseAnd);
BINARY_OPERATOR(BitwiseOr);
BINARY_OPERATOR(BitwiseXor);
BINARY_OPERATOR(LogicAnd);
BINARY_OPERATOR(LogicOr);

#endif
