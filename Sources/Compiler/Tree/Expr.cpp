#include "Expr.h"
#include "Compiler/CompilerError.h"
#include <sstream>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Expr::Expr(SourceLocation* location)
    : mLocation(location)
    , mEvaluating(false)
{
}

uint8_t Expr::evaluateByte() const
{
    Value value = evaluateValue();

    if (value.bits == SignificantBits::NoMoreThan8)
        value.truncateTo8Bit();
    else if (value.number < -128 || value.number > 255) {
        std::stringstream ss;
        ss << "value " << value.number << " (0x" << std::hex << value.number << ") does not fit into a byte";
        throw CompilerError(location(), ss.str());
    }

    if (value.number >= 0 || value.sign == Sign::Unsigned)
        return uint8_t(value.number);
    else
        return uint8_t(int8_t(value.number));
}

uint8_t Expr::evaluateByteOffset(int64_t nextAddress) const
{
    Value value = evaluateValue();
    if (value.bits != SignificantBits::All)
        value.truncateTo16Bit();

    int64_t offset = value.number - nextAddress;
    if (offset < -128 || offset > 127) {
        std::stringstream ss;
        ss << "value " << offset << " (0x" << std::hex << offset << ") does not fit into a byte";
        throw CompilerError(location(), ss.str());
    }

    if (offset >= 0 || value.sign == Sign::Unsigned)
        return uint8_t(offset);
    else
        return uint8_t(int8_t(offset));
}

uint16_t Expr::evaluateWord() const
{
    Value value = evaluateValue();

    if (value.bits == SignificantBits::NoMoreThan8 || value.bits == SignificantBits::NoMoreThan16)
        value.truncateTo16Bit();
    else if (value.number < -32768 || value.number > 65535) {
        std::stringstream ss;
        ss << "value " << value.number << " (0x" << std::hex << value.number << ") does not fit into a word";
        throw CompilerError(location(), ss.str());
    }

    if (value.number >= 0 || value.sign == Sign::Unsigned)
        return uint16_t(value.number);
    else
        return uint16_t(int16_t(value.number));
}

uint32_t Expr::evaluateDWord() const
{
    Value value = evaluateValue();

    if (value.bits == SignificantBits::NoMoreThan8 || value.bits == SignificantBits::NoMoreThan16)
        value.truncateTo32Bit();
    else if (value.number < -int64_t(0x80000000) || value.number > 0xffffffff) {
        std::stringstream ss;
        ss << "value " << value.number << " (0x" << std::hex << value.number << ") does not fit into a dword";
        throw CompilerError(location(), ss.str());
    }

    if (value.number >= 0 || value.sign == Sign::Unsigned)
        return uint32_t(value.number);
    else
        return uint32_t(int32_t(value.number));
}

Value Expr::evaluateValue() const
{
    class MarkAsEvaluating
    {
    public:
        explicit MarkAsEvaluating(const Expr* expr)
            : mExpr(expr)
        {
            if (mExpr->mEvaluating)
                throw CompilerError(mExpr->location(), "hit circular dependency while evaluating expression");
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


    MarkAsEvaluating mark(this);
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

Value ExprNumber::evaluate() const
{
    return Value(mValue);
}

Value ExprIdentifier::evaluate() const
{
    // FIXME
    std::stringstream ss;
    ss << "Use of undeclared identifier '" << mName << "'";
    throw CompilerError(location(), ss.str());
}

Value ExprConditional::evaluate() const
{
    Value a = mCondition->evaluateValue();
    return (a.number != 0 ? mThen->evaluateValue() : mElse->evaluateValue());
}

Value ExprNegate::evaluate() const
{
    Value value = mOperand->evaluateValue();
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

Value ExprBitwiseNot::evaluate() const
{
    Value operand = mOperand->evaluateValue();
    if (operand.sign == Sign::Unsigned)
        return Value(~operand.number & 0x7fffffffffffffffll, operand.sign, operand.bits);
    return Value(~operand.number, operand.sign, operand.bits);
}

Value ExprLogicNot::evaluate() const
{
    Value operand = mOperand->evaluateValue();
    return Value(!operand.number, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

Value ExprAdd::evaluate() const
{
    Value a = mOperand1->evaluateValue();
    Value b = mOperand2->evaluateValue();
    return smartEvaluate<false>([](int64_t a, int64_t b){ return a + b; }, a, b);
}

Value ExprSubtract::evaluate() const
{
    Value a = mOperand1->evaluateValue();
    Value b = mOperand2->evaluateValue();
    return smartEvaluate<true>([](int64_t a, int64_t b){ return a - b; }, a, b);
}

Value ExprMultiply::evaluate() const
{
    Value a = mOperand1->evaluateValue();
    Value b = mOperand2->evaluateValue();
    return smartEvaluate<false>([](int64_t a, int64_t b){ return a * b; }, a, b);
}

Value ExprDivide::evaluate() const
{
    Value a = mOperand1->evaluateValue();
    Value b = mOperand2->evaluateValue();
    return smartEvaluate<false>([](int64_t a, int64_t b){ return a / b; }, a, b);
}

Value ExprModulo::evaluate() const
{
    Value a = mOperand1->evaluateValue();
    Value b = mOperand2->evaluateValue();
    return smartEvaluate<false>([](int64_t a, int64_t b){ return a % b; }, a, b);
}

Value ExprShiftLeft::evaluate() const
{
    Value a = mOperand1->evaluateValue();
    Value b = mOperand2->evaluateValue();

    if (b.number < 0) {
        b.truncateToSignificantBits();
        if (b.number < 0)
            throw CompilerError(location(), "negative shift count for operator '<<'");
    }

    if (b.number > 64) {
        b.truncateToSignificantBits();
        if (b.number > 64)
            throw CompilerError(location(), "shift count is too large for operator '<<'");
    }

    return smartEvaluate<false>([](int64_t a, int64_t b){ return a << b; }, a, b);
}

Value ExprShiftRight::evaluate() const
{
    Value a = mOperand1->evaluateValue();
    Value b = mOperand2->evaluateValue();

    if (b.number < 0) {
        b.truncateToSignificantBits();
        if (b.number < 0)
            throw CompilerError(location(), "negative shift count for operator '>>'");
    }

    if (b.number > 64) {
        b.truncateToSignificantBits();
        if (b.number > 64)
            throw CompilerError(location(), "shift count is too large for operator '>>'");
    }

    return smartEvaluate<false>([](int64_t a, int64_t b){ return a >> b; }, a, b);
}

Value ExprLess::evaluate() const
{
    Value a = mOperand1->evaluateValue();
    Value b = mOperand2->evaluateValue();
    return Value(a.number < b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

Value ExprLessEqual::evaluate() const
{
    Value a = mOperand1->evaluateValue();
    Value b = mOperand2->evaluateValue();
    return Value(a.number <= b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

Value ExprGreater::evaluate() const
{
    Value a = mOperand1->evaluateValue();
    Value b = mOperand2->evaluateValue();
    return Value(a.number > b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

Value ExprGreaterEqual::evaluate() const
{
    Value a = mOperand1->evaluateValue();
    Value b = mOperand2->evaluateValue();
    return Value(a.number >= b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

Value ExprEqual::evaluate() const
{
    Value a = mOperand1->evaluateValue();
    Value b = mOperand2->evaluateValue();
    return Value(a.number == b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

Value ExprNotEqual::evaluate() const
{
    Value a = mOperand1->evaluateValue();
    Value b = mOperand2->evaluateValue();
    return Value(a.number != b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

Value ExprBitwiseAnd::evaluate() const
{
    Value a = mOperand1->evaluateValue();
    Value b = mOperand2->evaluateValue();
    auto bits = (a.bits > b.bits ? a.bits : b.bits);
    auto sign = (a.sign == Sign::Signed || b.sign == Sign::Signed ? Sign::Signed : Sign::Unsigned);
    return Value(a.number & b.number, sign, bits);
}

Value ExprBitwiseOr::evaluate() const
{
    Value a = mOperand1->evaluateValue();
    Value b = mOperand2->evaluateValue();
    auto bits = (a.bits > b.bits ? a.bits : b.bits);
    auto sign = (a.sign == Sign::Signed || b.sign == Sign::Signed ? Sign::Signed : Sign::Unsigned);
    return Value(a.number | b.number, sign, bits);
}

Value ExprBitwiseXor::evaluate() const
{
    Value a = mOperand1->evaluateValue();
    Value b = mOperand2->evaluateValue();
    auto bits = (a.bits > b.bits ? a.bits : b.bits);
    auto sign = (a.sign == Sign::Signed || b.sign == Sign::Signed ? Sign::Signed : Sign::Unsigned);
    return Value(a.number ^ b.number, sign, bits);
}

Value ExprLogicAnd::evaluate() const
{
    Value a = mOperand1->evaluateValue();
    Value b = mOperand2->evaluateValue();
    return Value(a.number && b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

Value ExprLogicOr::evaluate() const
{
    Value a = mOperand1->evaluateValue();
    Value b = mOperand2->evaluateValue();
    return Value(a.number || b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}
