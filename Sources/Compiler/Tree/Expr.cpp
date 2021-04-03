#include "Expr.h"
#include "Compiler/Assembler/Label.h"
#include "Compiler/Assembler/AssemblerContext.h"
#include "Compiler/Tree/Symbol.h"
#include "Compiler/CompilerError.h"

class Expr::MarkAsEvaluating
{
public:
    explicit MarkAsEvaluating(const Expr* expr)
        : mExpr(expr)
    {
        if (mExpr->mEvaluating)
            throw CompilerError(mExpr->location(), "hit circular dependency while evaluating expression.");
        mExpr->mEvaluating = true;
    }

    ~MarkAsEvaluating() noexcept
    {
        mExpr->mEvaluating = false;
    }

private:
    const Expr* mExpr;

    DISABLE_COPY(MarkAsEvaluating);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Expr::Expr(SourceLocation* location)
    : mLocation(location)
    , mEvaluating(false)
{
}

bool Expr::isNegate() const
{
    return false;
}

bool Expr::canEvaluateValue(const int64_t* currentAddress, std::unique_ptr<CompilerError>& resolveError) const
{
    MarkAsEvaluating mark(this);
    mCurrentAddress = currentAddress;
    return canEvaluate(resolveError);
}

uint8_t Expr::evaluateByte(const int64_t* currentAddress) const
{
    Value value = evaluateValue(currentAddress);

    if (value.bits == SignificantBits::NoMoreThan8)
        value.truncateTo8Bit();
    else if (value.number < -128 || value.number > 255) {
        std::stringstream ss;
        ss << "value " << value.number << " (0x" << std::hex << value.number << ") does not fit into a byte.";
        throw CompilerError(location(), ss.str());
    }

    if (value.number >= 0 || value.sign == Sign::Unsigned)
        return uint8_t(value.number);
    else
        return uint8_t(int8_t(value.number));
}

uint8_t Expr::evaluateByteOffset(int64_t nextAddress, const int64_t* currentAddress) const
{
    if (!currentAddress)
        throw CompilerError(location(), "byte offset cannot be evaluated at this point.");

    Value value = evaluateValue(currentAddress);
    if (value.bits != SignificantBits::All)
        value.truncateTo16Bit();

    int64_t offset = value.number - nextAddress;
    if (offset < -128 || offset > 127) {
        std::stringstream ss;
        ss << "value " << offset << " (0x" << std::hex << offset << ") does not fit into a byte.";
        throw CompilerError(location(), ss.str());
    }

    if (offset >= 0 || value.sign == Sign::Unsigned)
        return uint8_t(offset);
    else
        return uint8_t(int8_t(offset));
}

uint16_t Expr::evaluateWord(const int64_t* currentAddress) const
{
    Value value = evaluateValue(currentAddress);

    if (value.bits == SignificantBits::NoMoreThan8 || value.bits == SignificantBits::NoMoreThan16)
        value.truncateTo16Bit();
    else if (value.number < -32768 || value.number > 65535) {
        std::stringstream ss;
        ss << "value " << value.number << " (0x" << std::hex << value.number << ") does not fit into a word.";
        throw CompilerError(location(), ss.str());
    }

    if (value.number >= 0 || value.sign == Sign::Unsigned)
        return uint16_t(value.number);
    else
        return uint16_t(int16_t(value.number));
}

uint16_t Expr::evaluateUnsignedWord(const int64_t* currentAddress) const
{
    Value value = evaluateValue(currentAddress);

    if (value.bits == SignificantBits::NoMoreThan8 || value.bits == SignificantBits::NoMoreThan16)
        value.truncateTo16Bit();
    else if (value.number < 0) {
        std::stringstream ss;
        ss << "negative value is not allowed in this context.";
        throw CompilerError(location(), ss.str());
    } else if (value.number > 65535) {
        std::stringstream ss;
        ss << "value " << value.number << " (0x" << std::hex << value.number << ") does not fit into a word.";
        throw CompilerError(location(), ss.str());
    }

    return uint16_t(value.number);
}

uint32_t Expr::evaluateDWord(const int64_t* currentAddress) const
{
    Value value = evaluateValue(currentAddress);

    if (value.bits == SignificantBits::NoMoreThan8 || value.bits == SignificantBits::NoMoreThan16)
        value.truncateTo32Bit();
    else if (value.number < -int64_t(0x80000000) || value.number > 0xffffffff) {
        std::stringstream ss;
        ss << "value " << value.number << " (0x" << std::hex << value.number << ") does not fit into a dword.";
        throw CompilerError(location(), ss.str());
    }

    if (value.number >= 0 || value.sign == Sign::Unsigned)
        return uint32_t(value.number);
    else
        return uint32_t(int32_t(value.number));
}

Value Expr::evaluateValue(const int64_t* currentAddress) const
{
    MarkAsEvaluating mark(this);
    mCurrentAddress = currentAddress;
    return evaluate();
}

template <bool SUB, typename T> Value Expr::smartEvaluate(T&& operatr, Value a, Value b)
{
    auto bits = (a.bits > b.bits ? a.bits : b.bits);
    Sign sign = (a.sign == Sign::Signed || b.sign == Sign::Signed ? Sign::Signed : Sign::Unsigned);

    switch (bits) {
        case SignificantBits::NoMoreThan8: {
            if (sign == Sign::Unsigned) {
                uint8_t aa = uint8_t(a.number & 0xff);
                uint8_t bb = uint8_t(b.number & 0xff);
                if (!SUB || bb <= aa)
                    return Value(operatr(a.number, b.number), sign, Value::significantBitsForNumber(operatr(aa, bb)));
            }
            int8_t aa = int8_t(a.number & 0xff);
            int8_t bb = int8_t(b.number & 0xff);
            return Value(operatr(a.number, b.number), Sign::Signed, Value::significantBitsForNumber(operatr(aa, bb)));
        }

        case SignificantBits::NoMoreThan16: {
            if (sign == Sign::Unsigned) {
                uint16_t aa = uint16_t(a.number & 0xffff);
                uint16_t bb = uint16_t(b.number & 0xffff);
                if (!SUB || bb <= aa)
                    return Value(operatr(a.number, b.number), sign, Value::significantBitsForNumber(operatr(aa, bb)));
            }
            int16_t aa = int16_t(a.number & 0xffff);
            int16_t bb = int16_t(b.number & 0xffff);
            return Value(operatr(a.number, b.number), Sign::Signed, Value::significantBitsForNumber(operatr(aa, bb)));
        }

        case SignificantBits::All:
            break;
    }

    return Value(operatr(a.number, b.number), sign);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprCurrentAddress::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    if (!mLabel) {
        if (!mCurrentAddress) {
            resolveError = std::make_unique<CompilerError>(location(),
                "current address is not available at this point.");
            return false;
        }
    } else {
        if (!mLabel->hasAddress()) {
            std::stringstream ss;
            ss << "value of '$' in EQU is not available at this point.";
            resolveError = std::make_unique<CompilerError>(location(), ss.str());
            return false;
        }
    }
    return true;
}

Value ExprCurrentAddress::evaluate() const
{
    if (!mLabel) {
        if (!mCurrentAddress)
            throw CompilerError(location(), "current address is not available at this point.");
        return Value(*mCurrentAddress);
    } else {
        if (!mLabel->hasAddress()) {
            std::stringstream ss;
            ss << "value of '$' in EQU is not available at this point.";
            throw CompilerError(location(), ss.str());
        }
        return Value(mLabel->addressValue(), Sign::Unsigned, SignificantBits::NoMoreThan16);
    }
}

void ExprCurrentAddress::toString(std::stringstream& ss) const
{
    ss << '$';
}

void ExprCurrentAddress::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    if (mLabel)
        throw CompilerError(location(), "internal compiler error: '$' is already replaced with label.");
    mLabel = context->addEphemeralLabel(location());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprNumber::canEvaluate(std::unique_ptr<CompilerError>&) const
{
    return true;
}

Value ExprNumber::evaluate() const
{
    return Value(mValue);
}

void ExprNumber::toString(std::stringstream& ss) const
{
    ss << mValue;
}

void ExprNumber::replaceCurrentAddressWithLabel(AssemblerContext*)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprIdentifier::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    auto symbol = mSymbolTable->findSymbol(mName);
    if (!symbol) {
        std::stringstream ss;
        ss << "use of undeclared identifier '" << mName << "'.";
        throw CompilerError(location(), ss.str());
    }

    switch (symbol->type()) {
        case Symbol::Constant:
            return static_cast<ConstantSymbol*>(symbol)->value()->canEvaluateValue(mCurrentAddress, resolveError);

        case Symbol::ConditionalConstant:
            return static_cast<ConditionalConstantSymbol*>(symbol)->canEvaluateValue(mCurrentAddress, resolveError);

        case Symbol::Label: {
            auto label = static_cast<LabelSymbol*>(symbol)->label();
            if (!label->hasAddress()) {
                std::stringstream ss;
                ss << "unable to resolve address for label \"" << label->name() << "\".";
                resolveError = std::make_unique<CompilerError>(location(), ss.str());
                return false;
            }
            return true;
        }

        case Symbol::ConditionalLabel: {
            auto labelSymbol = static_cast<ConditionalLabelSymbol*>(symbol);
            if (!labelSymbol->canEvaluateValue(mCurrentAddress, resolveError))
                return false;
            Label* label = labelSymbol->label(location(), mCurrentAddress);
            if (!label) {
                std::stringstream ss;
                ss << "unable to resolve label \"" << labelSymbol->name() << "\".";
                resolveError = std::make_unique<CompilerError>(location(), ss.str());
                return false;
            }
            if (!label->hasAddress()) {
                std::stringstream ss;
                ss << "unable to resolve address for label \"" << labelSymbol->name() << "\".";
                resolveError = std::make_unique<CompilerError>(location(), ss.str());
                return false;
            }
            return true;
        }
    }

    throw CompilerError(symbol->location(), "internal compiler error: invalid symbol type.");
}

Value ExprIdentifier::evaluate() const
{
    auto symbol = mSymbolTable->findSymbol(mName);
    if (!symbol) {
        std::stringstream ss;
        ss << "use of undeclared identifier '" << mName << "'.";
        throw CompilerError(location(), ss.str());
    }

    switch (symbol->type()) {
        case Symbol::Constant:
            return static_cast<ConstantSymbol*>(symbol)->value()->evaluateValue(mCurrentAddress);

        case Symbol::ConditionalConstant: {
            Expr* expr = static_cast<ConditionalConstantSymbol*>(symbol)->expr(location(), mCurrentAddress);
            if (!expr) {
                std::stringstream ss;
                ss << "unable to resolve symbol \"" << symbol->name() << "\".";
                throw CompilerError(location(), ss.str());
            }
            return expr->evaluateValue(mCurrentAddress);
        }

        case Symbol::Label: {
            auto label = static_cast<LabelSymbol*>(symbol)->label();
            if (!label->hasAddress()) {
                std::stringstream ss;
                ss << "value for label \"" << label->name() << "\" is not available in this context.";
                throw CompilerError(location(), ss.str());
            }
            return Value(label->addressValue(), Sign::Unsigned, SignificantBits::NoMoreThan16);
        }

        case Symbol::ConditionalLabel: {
            auto label = static_cast<ConditionalLabelSymbol*>(symbol)->label(location(), mCurrentAddress);
            if (!label) {
                std::stringstream ss;
                ss << "unable to resolve label \"" << symbol->name() << "\".";
                throw CompilerError(location(), ss.str());
            }
            if (!label->hasAddress()) {
                std::stringstream ss;
                ss << "value for label \"" << label->name() << "\" is not available in this context.";
                throw CompilerError(location(), ss.str());
            }
            return Value(label->addressValue(), Sign::Unsigned, SignificantBits::NoMoreThan16);
        }
    }

    throw CompilerError(symbol->location(), "internal compiler error: invalid symbol type.");
}

void ExprIdentifier::toString(std::stringstream& ss) const
{
    ss << mName;
}

void ExprIdentifier::replaceCurrentAddressWithLabel(AssemblerContext*)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprConditional::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mCondition->canEvaluateValue(mCurrentAddress, resolveError)
        && mThen->canEvaluateValue(mCurrentAddress, resolveError)
        && mElse->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprConditional::evaluate() const
{
    Value a = mCondition->evaluateValue(mCurrentAddress);
    return (a.number != 0 ? mThen->evaluateValue(mCurrentAddress) : mElse->evaluateValue(mCurrentAddress));
}

void ExprConditional::toString(std::stringstream& ss) const
{
    mCondition->toString(ss);
    ss << " ? ";
    mThen->toString(ss);
    ss << " : ";
    mElse->toString(ss);
}

void ExprConditional::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mCondition->replaceCurrentAddressWithLabel(context);
    mThen->replaceCurrentAddressWithLabel(context);
    mElse->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprNegate::isNegate() const
{
    return true;
}

bool ExprNegate::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprNegate::evaluate() const
{
    Value value = mOperand->evaluateValue(mCurrentAddress);
    switch (value.bits) {
        case SignificantBits::NoMoreThan8:
            if (value.sign == Sign::Unsigned) {
                uint8_t x = uint8_t(value.number & 0xff);
                if (x <= 0x80)
                    return Value(-value.number, Sign::Signed, SignificantBits::NoMoreThan8);
            } else {
                int8_t x = int8_t(value.number & 0xff);
                if (x != -128) // +128 does not fit into 8 bits
                    return Value(-value.number, Sign::Signed, SignificantBits::NoMoreThan8);
            }
            return Value(-value.number, Sign::Signed, SignificantBits::NoMoreThan16);

        case SignificantBits::NoMoreThan16:
            if (value.sign == Sign::Unsigned) {
                uint16_t x = uint16_t(value.number & 0xffff);
                if (x <= 0x8000)
                    return Value(-value.number, Sign::Signed, SignificantBits::NoMoreThan16);
            } else {
                int16_t x = int16_t(value.number & 0xffff);
                if (x != -32768) // +32768 does not fit into 16 bits
                    return Value(-value.number, Sign::Signed, SignificantBits::NoMoreThan16);
            }
            break;

        case SignificantBits::All:
            break;
    }

    return Value(-value.number, Sign::Signed);
}

void ExprNegate::toString(std::stringstream& ss) const
{
    ss << '-';
    mOperand->toString(ss);
}

void ExprNegate::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprBitwiseNot::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprBitwiseNot::evaluate() const
{
    Value operand = mOperand->evaluateValue(mCurrentAddress);
    if (operand.sign == Sign::Unsigned)
        return Value(~operand.number & 0x7fffffffffffffffll, operand.sign, operand.bits);
    return Value(~operand.number, operand.sign, operand.bits);
}

void ExprBitwiseNot::toString(std::stringstream& ss) const
{
    ss << '~';
    mOperand->toString(ss);
}

void ExprBitwiseNot::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprLogicNot::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprLogicNot::evaluate() const
{
    Value operand = mOperand->evaluateValue(mCurrentAddress);
    return Value(!operand.number, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

void ExprLogicNot::toString(std::stringstream& ss) const
{
    ss << '!';
    mOperand->toString(ss);
}

void ExprLogicNot::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprAdd::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprAdd::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress);
    Value b = mOperand2->evaluateValue(mCurrentAddress);
    return smartEvaluate<false>([](int64_t a, int64_t b){ return a + b; }, a, b);
}

void ExprAdd::toString(std::stringstream& ss) const
{
    mOperand1->toString(ss);
    ss << " + ";
    mOperand2->toString(ss);
}

void ExprAdd::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand1->replaceCurrentAddressWithLabel(context);
    mOperand2->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprSubtract::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprSubtract::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress);
    Value b = mOperand2->evaluateValue(mCurrentAddress);
    return smartEvaluate<true>([](int64_t a, int64_t b){ return a - b; }, a, b);
}

void ExprSubtract::toString(std::stringstream& ss) const
{
    mOperand1->toString(ss);
    ss << " - ";
    mOperand2->toString(ss);
}

void ExprSubtract::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand1->replaceCurrentAddressWithLabel(context);
    mOperand2->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprMultiply::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprMultiply::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress);
    Value b = mOperand2->evaluateValue(mCurrentAddress);
    return smartEvaluate<false>([](int64_t a, int64_t b){ return a * b; }, a, b);
}

void ExprMultiply::toString(std::stringstream& ss) const
{
    mOperand1->toString(ss);
    ss << " * ";
    mOperand2->toString(ss);
}

void ExprMultiply::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand1->replaceCurrentAddressWithLabel(context);
    mOperand2->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprDivide::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprDivide::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress);
    Value b = mOperand2->evaluateValue(mCurrentAddress);
    return smartEvaluate<false>([](int64_t a, int64_t b){ return a / b; }, a, b);
}

void ExprDivide::toString(std::stringstream& ss) const
{
    mOperand1->toString(ss);
    ss << " / ";
    mOperand2->toString(ss);
}

void ExprDivide::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand1->replaceCurrentAddressWithLabel(context);
    mOperand2->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprModulo::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprModulo::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress);
    Value b = mOperand2->evaluateValue(mCurrentAddress);
    return smartEvaluate<false>([](int64_t a, int64_t b){ return a % b; }, a, b);
}

void ExprModulo::toString(std::stringstream& ss) const
{
    mOperand1->toString(ss);
    ss << " % ";
    mOperand2->toString(ss);
}

void ExprModulo::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand1->replaceCurrentAddressWithLabel(context);
    mOperand2->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprShiftLeft::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprShiftLeft::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress);
    Value b = mOperand2->evaluateValue(mCurrentAddress);

    if (b.number < 0) {
        b.truncateToSignificantBits();
        if (b.number < 0)
            throw CompilerError(location(), "negative shift count for operator '<<'.");
    }

    if (b.number > 64) {
        b.truncateToSignificantBits();
        if (b.number > 64)
            throw CompilerError(location(), "shift count is too large for operator '<<'.");
    }

    return smartEvaluate<false>([](int64_t a, int64_t b){ return a << b; }, a, b);
}

void ExprShiftLeft::toString(std::stringstream& ss) const
{
    mOperand1->toString(ss);
    ss << " << ";
    mOperand2->toString(ss);
}

void ExprShiftLeft::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand1->replaceCurrentAddressWithLabel(context);
    mOperand2->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprShiftRight::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprShiftRight::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress);
    Value b = mOperand2->evaluateValue(mCurrentAddress);

    if (b.number < 0) {
        b.truncateToSignificantBits();
        if (b.number < 0)
            throw CompilerError(location(), "negative shift count for operator '>>'.");
    }

    if (b.number > 64) {
        b.truncateToSignificantBits();
        if (b.number > 64)
            throw CompilerError(location(), "shift count is too large for operator '>>'.");
    }

    return smartEvaluate<false>([](int64_t a, int64_t b){ return a >> b; }, a, b);
}

void ExprShiftRight::toString(std::stringstream& ss) const
{
    mOperand1->toString(ss);
    ss << " >> ";
    mOperand2->toString(ss);
}

void ExprShiftRight::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand1->replaceCurrentAddressWithLabel(context);
    mOperand2->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprLess::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprLess::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress);
    Value b = mOperand2->evaluateValue(mCurrentAddress);
    return Value(a.number < b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

void ExprLess::toString(std::stringstream& ss) const
{
    mOperand1->toString(ss);
    ss << " < ";
    mOperand2->toString(ss);
}

void ExprLess::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand1->replaceCurrentAddressWithLabel(context);
    mOperand2->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprLessEqual::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprLessEqual::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress);
    Value b = mOperand2->evaluateValue(mCurrentAddress);
    return Value(a.number <= b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

void ExprLessEqual::toString(std::stringstream& ss) const
{
    mOperand1->toString(ss);
    ss << " <= ";
    mOperand2->toString(ss);
}

void ExprLessEqual::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand1->replaceCurrentAddressWithLabel(context);
    mOperand2->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprGreater::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprGreater::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress);
    Value b = mOperand2->evaluateValue(mCurrentAddress);
    return Value(a.number > b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

void ExprGreater::toString(std::stringstream& ss) const
{
    mOperand1->toString(ss);
    ss << " > ";
    mOperand2->toString(ss);
}

void ExprGreater::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand1->replaceCurrentAddressWithLabel(context);
    mOperand2->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprGreaterEqual::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprGreaterEqual::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress);
    Value b = mOperand2->evaluateValue(mCurrentAddress);
    return Value(a.number >= b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

void ExprGreaterEqual::toString(std::stringstream& ss) const
{
    mOperand1->toString(ss);
    ss << " >= ";
    mOperand2->toString(ss);
}

void ExprGreaterEqual::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand1->replaceCurrentAddressWithLabel(context);
    mOperand2->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprEqual::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprEqual::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress);
    Value b = mOperand2->evaluateValue(mCurrentAddress);
    return Value(a.number == b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

void ExprEqual::toString(std::stringstream& ss) const
{
    mOperand1->toString(ss);
    ss << " == ";
    mOperand2->toString(ss);
}

void ExprEqual::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand1->replaceCurrentAddressWithLabel(context);
    mOperand2->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprNotEqual::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprNotEqual::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress);
    Value b = mOperand2->evaluateValue(mCurrentAddress);
    return Value(a.number != b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

void ExprNotEqual::toString(std::stringstream& ss) const
{
    mOperand1->toString(ss);
    ss << " != ";
    mOperand2->toString(ss);
}

void ExprNotEqual::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand1->replaceCurrentAddressWithLabel(context);
    mOperand2->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprBitwiseAnd::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprBitwiseAnd::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress);
    Value b = mOperand2->evaluateValue(mCurrentAddress);
    auto bits = (a.bits > b.bits ? a.bits : b.bits);
    auto sign = (a.sign == Sign::Signed || b.sign == Sign::Signed ? Sign::Signed : Sign::Unsigned);
    return Value(a.number & b.number, sign, bits);
}

void ExprBitwiseAnd::toString(std::stringstream& ss) const
{
    mOperand1->toString(ss);
    ss << " & ";
    mOperand2->toString(ss);
}

void ExprBitwiseAnd::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand1->replaceCurrentAddressWithLabel(context);
    mOperand2->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprBitwiseOr::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprBitwiseOr::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress);
    Value b = mOperand2->evaluateValue(mCurrentAddress);
    auto bits = (a.bits > b.bits ? a.bits : b.bits);
    auto sign = (a.sign == Sign::Signed || b.sign == Sign::Signed ? Sign::Signed : Sign::Unsigned);
    return Value(a.number | b.number, sign, bits);
}

void ExprBitwiseOr::toString(std::stringstream& ss) const
{
    mOperand1->toString(ss);
    ss << " | ";
    mOperand2->toString(ss);
}

void ExprBitwiseOr::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand1->replaceCurrentAddressWithLabel(context);
    mOperand2->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprBitwiseXor::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprBitwiseXor::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress);
    Value b = mOperand2->evaluateValue(mCurrentAddress);
    auto bits = (a.bits > b.bits ? a.bits : b.bits);
    auto sign = (a.sign == Sign::Signed || b.sign == Sign::Signed ? Sign::Signed : Sign::Unsigned);
    return Value(a.number ^ b.number, sign, bits);
}

void ExprBitwiseXor::toString(std::stringstream& ss) const
{
    mOperand1->toString(ss);
    ss << " ^ ";
    mOperand2->toString(ss);
}

void ExprBitwiseXor::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand1->replaceCurrentAddressWithLabel(context);
    mOperand2->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprLogicAnd::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprLogicAnd::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress);
    Value b = mOperand2->evaluateValue(mCurrentAddress);
    return Value(a.number && b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

void ExprLogicAnd::toString(std::stringstream& ss) const
{
    mOperand1->toString(ss);
    ss << " && ";
    mOperand2->toString(ss);
}

void ExprLogicAnd::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand1->replaceCurrentAddressWithLabel(context);
    mOperand2->replaceCurrentAddressWithLabel(context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprLogicOr::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, resolveError);
}

Value ExprLogicOr::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress);
    Value b = mOperand2->evaluateValue(mCurrentAddress);
    return Value(a.number || b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

void ExprLogicOr::toString(std::stringstream& ss) const
{
    mOperand1->toString(ss);
    ss << " || ";
    mOperand2->toString(ss);
}

void ExprLogicOr::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    mOperand1->replaceCurrentAddressWithLabel(context);
    mOperand2->replaceCurrentAddressWithLabel(context);
}
