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

    virtual bool isNegate() const;

    virtual void toString(std::stringstream& ss) const = 0;

    uint8_t evaluateByte() const;
    uint8_t evaluateByteOffset(int64_t nextAddress) const;
    uint16_t evaluateWord() const;
    uint16_t evaluateUnsignedWord() const;
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

class ExprCurrentAddress : public Expr
{
public:
    explicit ExprCurrentAddress(SourceLocation* location)
        : Expr(location)
    {
    }

    void toString(std::stringstream& ss) const override;

private:
    Value evaluate() const override;

    DISABLE_COPY(ExprCurrentAddress);
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

    void toString(std::stringstream& ss) const override;

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

    void toString(std::stringstream& ss) const override;

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

    void toString(std::stringstream& ss) const override;

private:
    Expr* mCondition;
    Expr* mThen;
    Expr* mElse;

    Value evaluate() const override;

    DISABLE_COPY(ExprConditional);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TREE_UNARY_OPERATOR(NAME) \
    class Expr##NAME : public Expr \
    { \
    public: \
        Expr##NAME(SourceLocation* location, Expr* operand) \
            : Expr(location) \
            , mOperand(operand) \
        { \
        } \
        Expr* operand() const noexcept { return mOperand; } \
        void toString(std::stringstream& ss) const override; \
    private: \
        Expr* mOperand; \
        Value evaluate() const override; \
        DISABLE_COPY(Expr##NAME); \
    }

#define TREE_BINARY_OPERATOR(NAME) \
    class Expr##NAME : public Expr \
    { \
    public: \
        Expr##NAME(SourceLocation* location, Expr* op1, Expr* op2) \
            : Expr(location) \
            , mOperand1(op1) \
            , mOperand2(op2) \
        { \
        } \
        void toString(std::stringstream& ss) const override; \
    private: \
        Expr* mOperand1; \
        Expr* mOperand2; \
        Value evaluate() const override; \
        DISABLE_COPY(Expr##NAME); \
    }

class ExprNegate : public Expr
{
public:
    ExprNegate(SourceLocation* location, Expr* operand)
        : Expr(location)
        , mOperand(operand)
    {
    }

    bool isNegate() const override;

    Expr* operand() const noexcept { return mOperand; }

    void toString(std::stringstream& ss) const override;

private:
    Expr* mOperand;

    Value evaluate() const override;

    DISABLE_COPY(ExprNegate);
};

TREE_UNARY_OPERATOR(BitwiseNot);
TREE_UNARY_OPERATOR(LogicNot);

TREE_BINARY_OPERATOR(Add);
TREE_BINARY_OPERATOR(Subtract);
TREE_BINARY_OPERATOR(Multiply);
TREE_BINARY_OPERATOR(Divide);
TREE_BINARY_OPERATOR(Modulo);
TREE_BINARY_OPERATOR(ShiftLeft);
TREE_BINARY_OPERATOR(ShiftRight);
TREE_BINARY_OPERATOR(Less);
TREE_BINARY_OPERATOR(LessEqual);
TREE_BINARY_OPERATOR(Greater);
TREE_BINARY_OPERATOR(GreaterEqual);
TREE_BINARY_OPERATOR(Equal);
TREE_BINARY_OPERATOR(NotEqual);
TREE_BINARY_OPERATOR(BitwiseAnd);
TREE_BINARY_OPERATOR(BitwiseOr);
TREE_BINARY_OPERATOR(BitwiseXor);
TREE_BINARY_OPERATOR(LogicAnd);
TREE_BINARY_OPERATOR(LogicOr);

#endif
