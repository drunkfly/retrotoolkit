#ifndef COMPILER_TREE_EXPR_H
#define COMPILER_TREE_EXPR_H

#include "Common/GC.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/Tree/SourceLocation.h"
#include "Compiler/Tree/Value.h"
#include "Compiler/Token.h"

class ExprIdentifier;
class AssemblerContext;
class Label;
class CompilerError;
class ISectionResolver;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Expr : public GCObject
{
public:
    explicit Expr(SourceLocation* location);

    SourceLocation* location() const { return mLocation; }

    virtual bool isNegate() const;

    virtual bool isHereVariable() const;
    virtual bool containsHereVariable() const = 0;

    virtual void toString(std::stringstream& ss) const = 0;

    virtual void replaceCurrentAddressWithLabel(AssemblerContext* context) = 0;

    bool canEvaluateValue(const int64_t* currentAddress,
        ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const;

    uint8_t evaluateByte(const int64_t* currentAddress, ISectionResolver* sectionResolver) const;
    uint8_t evaluateByteOffset(int64_t nextAddress,
        const int64_t* currentAddress, ISectionResolver* sectionResolver) const;
    uint16_t evaluateWord(const int64_t* currentAddress, ISectionResolver* sectionResolver) const;
    uint16_t evaluateUnsignedWord(const int64_t* currentAddress, ISectionResolver* sectionResolver) const;
    uint32_t evaluateDWord(const int64_t* currentAddress, ISectionResolver* sectionResolver) const;
    Value evaluateValue(const int64_t* currentAddress, ISectionResolver* sectionResolver) const;

protected:
    mutable const int64_t* mCurrentAddress;
    mutable ISectionResolver* mSectionResolver;

    virtual bool canEvaluate(std::unique_ptr<CompilerError>& resolveError) const = 0;
    virtual Value evaluate() const = 0;

    template <bool SUB, typename T> static Value smartEvaluate(T&& operatr, Value a, Value b);

private:
    class MarkAsEvaluating;

    SourceLocation* mLocation;
    mutable bool mEvaluating;

    DISABLE_COPY(Expr);
    friend class ExprIdentifier;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ExprCurrentAddress final : public Expr
{
public:
    explicit ExprCurrentAddress(SourceLocation* location)
        : Expr(location)
        , mLabel(nullptr)
    {
    }

    bool containsHereVariable() const override;

    void toString(std::stringstream& ss) const override;

    void replaceCurrentAddressWithLabel(AssemblerContext* context) override;

private:
    Label* mLabel;

    bool canEvaluate(std::unique_ptr<CompilerError>& resolveError) const override;
    Value evaluate() const override;

    DISABLE_COPY(ExprCurrentAddress);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ExprVariableHere final : public Expr
{
public:
    ExprVariableHere(const Token* name, Expr* initializer)
        : Expr(name->location())
        , mName(name)
        , mInitializer(initializer)
    {
    }

    const Token* variableName() const { return mName; }

    bool isHereVariable() const override;
    bool containsHereVariable() const override;

    void toString(std::stringstream& ss) const override;

    void replaceCurrentAddressWithLabel(AssemblerContext* context) override;

private:
    const Token* mName;
    Expr* mInitializer;

    bool canEvaluate(std::unique_ptr<CompilerError>& resolveError) const override;
    Value evaluate() const override;

    DISABLE_COPY(ExprVariableHere);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ExprNumber final : public Expr
{
public:
    ExprNumber(SourceLocation* location, int64_t value)
        : Expr(location)
        , mValue(value)
    {
    }

    bool containsHereVariable() const override;

    void toString(std::stringstream& ss) const override;

    void replaceCurrentAddressWithLabel(AssemblerContext* context) override;

private:
    int64_t mValue;

    bool canEvaluate(std::unique_ptr<CompilerError>& resolveError) const override;
    Value evaluate() const override;

    DISABLE_COPY(ExprNumber);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ExprIdentifier final : public Expr
{
public:
    ExprIdentifier(SourceLocation* location, SymbolTable* table, std::string name)
        : Expr(location)
        , mSymbolTable(table)
        , mName(std::move(name))
    {
        registerFinalizer();
    }

    bool containsHereVariable() const override;

    void toString(std::stringstream& ss) const override;

    void replaceCurrentAddressWithLabel(AssemblerContext* context) override;

private:
    SymbolTable* mSymbolTable;
    std::string mName;

    bool canEvaluate(std::unique_ptr<CompilerError>& resolveError) const override;
    Value evaluate() const override;

    DISABLE_COPY(ExprIdentifier);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ExprConditional final : public Expr
{
public:
    ExprConditional(SourceLocation* location, Expr* cond, Expr* opThen, Expr* opElse)
        : Expr(location)
        , mCondition(cond)
        , mThen(opThen)
        , mElse(opElse)
    {
    }

    bool containsHereVariable() const override;

    void toString(std::stringstream& ss) const override;

    void replaceCurrentAddressWithLabel(AssemblerContext* context) override;

private:
    Expr* mCondition;
    Expr* mThen;
    Expr* mElse;

    bool canEvaluate(std::unique_ptr<CompilerError>& resolveError) const override;
    Value evaluate() const override;

    DISABLE_COPY(ExprConditional);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TREE_SECTION_OPERATOR(NAME) \
    class Expr##NAME final : public Expr \
    { \
    public: \
        Expr##NAME(SourceLocation* location, const char* sectionName) \
            : Expr(location) \
            , mSectionName(sectionName) \
        { \
        } \
        const char* sectionName() const noexcept { return mSectionName; } \
        bool containsHereVariable() const override; \
        void toString(std::stringstream& ss) const override; \
        void replaceCurrentAddressWithLabel(AssemblerContext* context) override; \
    private: \
        const char* mSectionName; \
        bool canEvaluate(std::unique_ptr<CompilerError>& resolveError) const override; \
        Value evaluate() const override; \
        DISABLE_COPY(Expr##NAME); \
    }

TREE_SECTION_OPERATOR(AddressOfSection);
TREE_SECTION_OPERATOR(BaseOfSection);
TREE_SECTION_OPERATOR(SizeOfSection);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TREE_UNARY_OPERATOR(NAME) \
    class Expr##NAME final : public Expr \
    { \
    public: \
        Expr##NAME(SourceLocation* location, Expr* operand) \
            : Expr(location) \
            , mOperand(operand) \
        { \
        } \
        Expr* operand() const noexcept { return mOperand; } \
        bool containsHereVariable() const override; \
        void toString(std::stringstream& ss) const override; \
        void replaceCurrentAddressWithLabel(AssemblerContext* context) override; \
    private: \
        Expr* mOperand; \
        bool canEvaluate(std::unique_ptr<CompilerError>& resolveError) const override; \
        Value evaluate() const override; \
        DISABLE_COPY(Expr##NAME); \
    }

#define TREE_BINARY_OPERATOR(NAME) \
    class Expr##NAME final : public Expr \
    { \
    public: \
        Expr##NAME(SourceLocation* location, Expr* op1, Expr* op2) \
            : Expr(location) \
            , mOperand1(op1) \
            , mOperand2(op2) \
        { \
        } \
        bool containsHereVariable() const override; \
        void toString(std::stringstream& ss) const override; \
        void replaceCurrentAddressWithLabel(AssemblerContext* context) override; \
    private: \
        Expr* mOperand1; \
        Expr* mOperand2; \
        bool canEvaluate(std::unique_ptr<CompilerError>& resolveError) const override; \
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
    bool containsHereVariable() const override;

    Expr* operand() const noexcept { return mOperand; }

    void toString(std::stringstream& ss) const override;

    void replaceCurrentAddressWithLabel(AssemblerContext* context) override;

private:
    Expr* mOperand;

    bool canEvaluate(std::unique_ptr<CompilerError>& resolveError) const override;
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
