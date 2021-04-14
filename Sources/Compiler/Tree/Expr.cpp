#include "Expr.h"
#include "Compiler/Assembler/Label.h"
#include "Compiler/Assembler/AssemblerContext.h"
#include "Compiler/Linker/ISectionResolver.h"
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

bool Expr::isHereVariable() const
{
    return false;
}

bool Expr::canEvaluateValue(const int64_t* currentAddress,
    ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const
{
    MarkAsEvaluating mark(this);
    mCurrentAddress = currentAddress;
    mSectionResolver = sectionResolver;
    return canEvaluate(resolveError);
}

uint8_t Expr::evaluateByte(const int64_t* currentAddress, ISectionResolver* sectionResolver) const
{
    Value value = evaluateValue(currentAddress, sectionResolver);

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

uint8_t Expr::evaluateByteOffset(int64_t nextAddress,
    const int64_t* currentAddress, ISectionResolver* sectionResolver) const
{
    if (!currentAddress)
        throw CompilerError(location(), "byte offset cannot be evaluated in this context.");

    Value value = evaluateValue(currentAddress, sectionResolver);
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

uint16_t Expr::evaluateWord(const int64_t* currentAddress, ISectionResolver* sectionResolver) const
{
    Value value = evaluateValue(currentAddress, sectionResolver);

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

uint16_t Expr::evaluateUnsignedWord(const int64_t* currentAddress, ISectionResolver* sectionResolver) const
{
    Value value = evaluateValue(currentAddress, sectionResolver);

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

uint32_t Expr::evaluateDWord(const int64_t* currentAddress, ISectionResolver* sectionResolver) const
{
    Value value = evaluateValue(currentAddress, sectionResolver);

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

Value Expr::evaluateValue(const int64_t* currentAddress, ISectionResolver* sectionResolver) const
{
    MarkAsEvaluating mark(this);
    mCurrentAddress = currentAddress;
    mSectionResolver = sectionResolver;
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

bool ExprCurrentAddress::containsHereVariable() const
{
    return false;
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

bool ExprCurrentAddress::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    if (!mLabel) {
        if (!mCurrentAddress) {
            resolveError = std::make_unique<CompilerError>(location(),
                "current address is not available in this context.");
            return false;
        }
    } else {
        if (!mLabel->hasAddress()) {
            std::stringstream ss;
            ss << "value of '$' in EQU is not available in this context.";
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
            throw CompilerError(location(), "current address is not available in this context.");
        return Value(*mCurrentAddress);
    } else {
        if (!mLabel->hasAddress()) {
            std::stringstream ss;
            ss << "value of '$' in EQU is not available in this context.";
            throw CompilerError(location(), ss.str());
        }
        return Value(mLabel->addressValue(), Sign::Unsigned, SignificantBits::NoMoreThan16);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprVariableHere::isHereVariable() const
{
    return true;
}

bool ExprVariableHere::containsHereVariable() const
{
    return true;
}

void ExprVariableHere::toString(std::stringstream& ss) const
{
    ss << "{@here ";
    ss << mName->text();
    if (mInitializer) {
        ss << '=';
        mInitializer->toString(ss);
    }
    ss << '}';
}

void ExprVariableHere::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
    if (mInitializer)
        mInitializer->replaceCurrentAddressWithLabel(context);
}

bool ExprVariableHere::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    if (!mInitializer)
        return true;
    else if (!mInitializer->containsHereVariable())
        return mInitializer->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
    else {
        // Use exception here instead of `return false`, because this error is fatal
        throw CompilerError(location(), "@here variable cannot be used as initializer for another @here variable.");
    }
}

Value ExprVariableHere::evaluate() const
{
    if (!mInitializer)
        return Value(0);
    else if (!mInitializer->containsHereVariable())
        return mInitializer->evaluateValue(mCurrentAddress, mSectionResolver);
    else
        throw CompilerError(location(), "@here variable cannot be used as initializer for another @here variable.");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprNumber::containsHereVariable() const
{
    return false;
}

void ExprNumber::toString(std::stringstream& ss) const
{
    ss << mValue;
}

void ExprNumber::replaceCurrentAddressWithLabel(AssemblerContext*)
{
}

bool ExprNumber::canEvaluate(std::unique_ptr<CompilerError>&) const
{
    return true;
}

Value ExprNumber::evaluate() const
{
    return Value(mValue);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprIdentifier::containsHereVariable() const
{
    return false;
}

void ExprIdentifier::toString(std::stringstream& ss) const
{
    ss << mName;
}

void ExprIdentifier::replaceCurrentAddressWithLabel(AssemblerContext*)
{
}

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
            return static_cast<ConstantSymbol*>(symbol)->value()->
                canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);

        case Symbol::ConditionalConstant:
            return static_cast<ConditionalConstantSymbol*>(symbol)->
                canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);

        case Symbol::RepeatVariable:
            return true;

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
            if (!labelSymbol->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError))
                return false;
            Label* label = labelSymbol->label(location(), mCurrentAddress, mSectionResolver);
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
            return static_cast<ConstantSymbol*>(symbol)->value()->evaluateValue(mCurrentAddress, mSectionResolver);

        case Symbol::RepeatVariable:
            return *static_cast<RepeatVariableSymbol*>(symbol)->value();

        case Symbol::ConditionalConstant: {
            Expr* expr = static_cast<ConditionalConstantSymbol*>(symbol)->
                expr(location(), mCurrentAddress, mSectionResolver);
            if (!expr) {
                std::stringstream ss;
                ss << "unable to resolve symbol \"" << symbol->name() << "\".";
                throw CompilerError(location(), ss.str());
            }
            return expr->evaluateValue(mCurrentAddress, mSectionResolver);
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
            auto label = static_cast<ConditionalLabelSymbol*>(symbol)->
                label(location(), mCurrentAddress, mSectionResolver);
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprConditional::containsHereVariable() const
{
    return mCondition->containsHereVariable()
        || mThen->containsHereVariable()
        || mElse->containsHereVariable();
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

bool ExprConditional::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mCondition->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mThen->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mElse->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprConditional::evaluate() const
{
    Value a = mCondition->evaluateValue(mCurrentAddress, mSectionResolver);
    return (a.number != 0
        ? mThen->evaluateValue(mCurrentAddress, mSectionResolver)
        : mElse->evaluateValue(mCurrentAddress, mSectionResolver));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprAddressOfSection::containsHereVariable() const
{
    return false;
}

void ExprAddressOfSection::toString(std::stringstream& ss) const
{
    ss << "addressof(" << mSectionName << ")";
}

void ExprAddressOfSection::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
}

bool ExprAddressOfSection::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    uint64_t value = 0;
    if (!mSectionResolver || !mSectionResolver->tryResolveSectionAddress(location(), mSectionName, value)) {
        resolveError = std::make_unique<CompilerError>(location(),
            "section address is not available in this context.");
        return false;
    }
    return true;
}

Value ExprAddressOfSection::evaluate() const
{
    uint64_t value = 0;
    if (!mSectionResolver || !mSectionResolver->tryResolveSectionAddress(location(), mSectionName, value))
        throw CompilerError(location(), "section address is not available in this context.");
    return Value(value);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprBaseOfSection::containsHereVariable() const
{
    return false;
}

void ExprBaseOfSection::toString(std::stringstream& ss) const
{
    ss << "baseof(" << mSectionName << ")";
}

void ExprBaseOfSection::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
}

bool ExprBaseOfSection::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    uint64_t value = 0;
    if (!mSectionResolver || !mSectionResolver->tryResolveSectionBase(location(), mSectionName, value)) {
        resolveError = std::make_unique<CompilerError>(location(),
            "section base is not available in this context.");
        return false;
    }
    return true;
}

Value ExprBaseOfSection::evaluate() const
{
    uint64_t value = 0;
    if (!mSectionResolver || !mSectionResolver->tryResolveSectionBase(location(), mSectionName, value))
        throw CompilerError(location(), "section base is not available in this context.");
    return Value(value);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprSizeOfSection::containsHereVariable() const
{
    return false;
}

void ExprSizeOfSection::toString(std::stringstream& ss) const
{
    ss << "sizeof(" << mSectionName << ")";
}

void ExprSizeOfSection::replaceCurrentAddressWithLabel(AssemblerContext* context)
{
}

bool ExprSizeOfSection::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    uint64_t value = 0;
    if (!mSectionResolver || !mSectionResolver->tryResolveSectionSize(location(), mSectionName, value)) {
        resolveError = std::make_unique<CompilerError>(location(),
            "section size is not available in this context.");
        return false;
    }
    return true;
}

Value ExprSizeOfSection::evaluate() const
{
    uint64_t value = 0;
    if (!mSectionResolver || !mSectionResolver->tryResolveSectionSize(location(), mSectionName, value))
        throw CompilerError(location(), "section size is not available in this context.");
    return Value(value);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprNegate::isNegate() const
{
    return true;
}

bool ExprNegate::containsHereVariable() const
{
    return mOperand->containsHereVariable();
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

bool ExprNegate::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprNegate::evaluate() const
{
    Value value = mOperand->evaluateValue(mCurrentAddress, mSectionResolver);
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprBitwiseNot::containsHereVariable() const
{
    return mOperand->containsHereVariable();
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

bool ExprBitwiseNot::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprBitwiseNot::evaluate() const
{
    Value operand = mOperand->evaluateValue(mCurrentAddress, mSectionResolver);
    if (operand.sign == Sign::Unsigned)
        return Value(~operand.number & 0x7fffffffffffffffll, operand.sign, operand.bits);
    return Value(~operand.number, operand.sign, operand.bits);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprLogicNot::containsHereVariable() const
{
    return mOperand->containsHereVariable();
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

bool ExprLogicNot::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprLogicNot::evaluate() const
{
    Value operand = mOperand->evaluateValue(mCurrentAddress, mSectionResolver);
    return Value(!operand.number, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprAdd::containsHereVariable() const
{
    return mOperand1->containsHereVariable()
        || mOperand2->containsHereVariable();
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

bool ExprAdd::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprAdd::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress, mSectionResolver);
    Value b = mOperand2->evaluateValue(mCurrentAddress, mSectionResolver);
    return smartEvaluate<false>([](int64_t a, int64_t b){ return a + b; }, a, b);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprSubtract::containsHereVariable() const
{
    return mOperand1->containsHereVariable()
        || mOperand2->containsHereVariable();
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

bool ExprSubtract::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprSubtract::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress, mSectionResolver);
    Value b = mOperand2->evaluateValue(mCurrentAddress, mSectionResolver);
    return smartEvaluate<true>([](int64_t a, int64_t b){ return a - b; }, a, b);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprMultiply::containsHereVariable() const
{
    return mOperand1->containsHereVariable()
        || mOperand2->containsHereVariable();
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

bool ExprMultiply::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprMultiply::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress, mSectionResolver);
    Value b = mOperand2->evaluateValue(mCurrentAddress, mSectionResolver);
    return smartEvaluate<false>([](int64_t a, int64_t b){ return a * b; }, a, b);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprDivide::containsHereVariable() const
{
    return mOperand1->containsHereVariable()
        || mOperand2->containsHereVariable();
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

bool ExprDivide::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprDivide::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress, mSectionResolver);
    Value b = mOperand2->evaluateValue(mCurrentAddress, mSectionResolver);
    return smartEvaluate<false>([](int64_t a, int64_t b){ return a / b; }, a, b);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprModulo::containsHereVariable() const
{
    return mOperand1->containsHereVariable()
        || mOperand2->containsHereVariable();
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

bool ExprModulo::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprModulo::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress, mSectionResolver);
    Value b = mOperand2->evaluateValue(mCurrentAddress, mSectionResolver);
    return smartEvaluate<false>([](int64_t a, int64_t b){ return a % b; }, a, b);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprShiftLeft::containsHereVariable() const
{
    return mOperand1->containsHereVariable()
        || mOperand2->containsHereVariable();
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

bool ExprShiftLeft::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprShiftLeft::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress, mSectionResolver);
    Value b = mOperand2->evaluateValue(mCurrentAddress, mSectionResolver);

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprShiftRight::containsHereVariable() const
{
    return mOperand1->containsHereVariable()
        || mOperand2->containsHereVariable();
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

bool ExprShiftRight::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprShiftRight::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress, mSectionResolver);
    Value b = mOperand2->evaluateValue(mCurrentAddress, mSectionResolver);

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprLess::containsHereVariable() const
{
    return mOperand1->containsHereVariable()
        || mOperand2->containsHereVariable();
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

bool ExprLess::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprLess::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress, mSectionResolver);
    Value b = mOperand2->evaluateValue(mCurrentAddress, mSectionResolver);
    return Value(a.number < b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprLessEqual::containsHereVariable() const
{
    return mOperand1->containsHereVariable()
        || mOperand2->containsHereVariable();
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

bool ExprLessEqual::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprLessEqual::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress, mSectionResolver);
    Value b = mOperand2->evaluateValue(mCurrentAddress, mSectionResolver);
    return Value(a.number <= b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprGreater::containsHereVariable() const
{
    return mOperand1->containsHereVariable()
        || mOperand2->containsHereVariable();
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

bool ExprGreater::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprGreater::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress, mSectionResolver);
    Value b = mOperand2->evaluateValue(mCurrentAddress, mSectionResolver);
    return Value(a.number > b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprGreaterEqual::containsHereVariable() const
{
    return mOperand1->containsHereVariable()
        || mOperand2->containsHereVariable();
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

bool ExprGreaterEqual::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprGreaterEqual::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress, mSectionResolver);
    Value b = mOperand2->evaluateValue(mCurrentAddress, mSectionResolver);
    return Value(a.number >= b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprEqual::containsHereVariable() const
{
    return mOperand1->containsHereVariable()
        || mOperand2->containsHereVariable();
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

bool ExprEqual::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprEqual::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress, mSectionResolver);
    Value b = mOperand2->evaluateValue(mCurrentAddress, mSectionResolver);
    return Value(a.number == b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprNotEqual::containsHereVariable() const
{
    return mOperand1->containsHereVariable()
        || mOperand2->containsHereVariable();
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

bool ExprNotEqual::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprNotEqual::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress, mSectionResolver);
    Value b = mOperand2->evaluateValue(mCurrentAddress, mSectionResolver);
    return Value(a.number != b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprBitwiseAnd::containsHereVariable() const
{
    return mOperand1->containsHereVariable()
        || mOperand2->containsHereVariable();
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

bool ExprBitwiseAnd::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprBitwiseAnd::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress, mSectionResolver);
    Value b = mOperand2->evaluateValue(mCurrentAddress, mSectionResolver);
    auto bits = (a.bits > b.bits ? a.bits : b.bits);
    auto sign = (a.sign == Sign::Signed || b.sign == Sign::Signed ? Sign::Signed : Sign::Unsigned);
    return Value(a.number & b.number, sign, bits);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprBitwiseOr::containsHereVariable() const
{
    return mOperand1->containsHereVariable()
        || mOperand2->containsHereVariable();
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

bool ExprBitwiseOr::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprBitwiseOr::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress, mSectionResolver);
    Value b = mOperand2->evaluateValue(mCurrentAddress, mSectionResolver);
    auto bits = (a.bits > b.bits ? a.bits : b.bits);
    auto sign = (a.sign == Sign::Signed || b.sign == Sign::Signed ? Sign::Signed : Sign::Unsigned);
    return Value(a.number | b.number, sign, bits);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprBitwiseXor::containsHereVariable() const
{
    return mOperand1->containsHereVariable()
        || mOperand2->containsHereVariable();
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

bool ExprBitwiseXor::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprBitwiseXor::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress, mSectionResolver);
    Value b = mOperand2->evaluateValue(mCurrentAddress, mSectionResolver);
    auto bits = (a.bits > b.bits ? a.bits : b.bits);
    auto sign = (a.sign == Sign::Signed || b.sign == Sign::Signed ? Sign::Signed : Sign::Unsigned);
    return Value(a.number ^ b.number, sign, bits);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprLogicAnd::containsHereVariable() const
{
    return mOperand1->containsHereVariable()
        || mOperand2->containsHereVariable();
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

bool ExprLogicAnd::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprLogicAnd::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress, mSectionResolver);
    Value b = mOperand2->evaluateValue(mCurrentAddress, mSectionResolver);
    return Value(a.number && b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExprLogicOr::containsHereVariable() const
{
    return mOperand1->containsHereVariable()
        || mOperand2->containsHereVariable();
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

bool ExprLogicOr::canEvaluate(std::unique_ptr<CompilerError>& resolveError) const
{
    return mOperand1->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError)
        && mOperand2->canEvaluateValue(mCurrentAddress, mSectionResolver, resolveError);
}

Value ExprLogicOr::evaluate() const
{
    Value a = mOperand1->evaluateValue(mCurrentAddress, mSectionResolver);
    Value b = mOperand2->evaluateValue(mCurrentAddress, mSectionResolver);
    return Value(a.number || b.number ? 1 : 0, Sign::Unsigned, SignificantBits::NoMoreThan8);
}
