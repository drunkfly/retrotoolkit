#include "Instructions.Z80.h"
#include "Compiler/Linker/CodeEmitter.h"
#include "Compiler/Token.h"
#include "Compiler/CompilerError.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

StringSet Z80::RegisterNames = {
        "a",
        "b",
        "c",
        "d",
        "e",
        "h",
        "l",
        "i",
        "r",
        "ixh",
        "ixl",
        "iyh",
        "iyl",
        "bc",
        "de",
        "hl",
        "sp",
        "ix",
        "iy",
        "af",
    };

StringSet Z80::ConditionNames = {
        "c",
        "nc",
        "z",
        "nz",
        "m",
        "p",
        "pe",
        "po",
    };

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Z80::Opcode::isZ80Opcode() const { return true; }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::bit::toString(std::stringstream& ss) const
{
    mValue->toString(ss);
}

bool Z80::bit::tryParse(ParsingContext* c)
{
    return c->expression(mValue, &RegisterNames, &ConditionNames, false);
}

bool Z80::bit::canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const
{
    return mValue->canEvaluateValue(nextAddress, resolveError);
}

int Z80::bit::value(int64_t currentAddress, uint8_t baseByte) const
{
    Value value = mValue->evaluateValue(&currentAddress);
    if (value.number < 0 || value.number > 7)
        throw CompilerError(mValue->location(), "bit index is out of range.");
    return uint8_t(baseByte | (uint8_t(value.number) << 3));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::byte::toString(std::stringstream& ss) const
{
    mValue->toString(ss);
}

bool Z80::byte::tryParse(ParsingContext* c)
{
    return c->expression(mValue, &RegisterNames, &ConditionNames, false);
}

bool Z80::byte::canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const
{
    return mValue->canEvaluateValue(nextAddress, resolveError);
}

int Z80::byte::value(int64_t currentAddress) const
{
    return mValue->evaluateByte(&currentAddress);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::word::toString(std::stringstream& ss) const
{
    mValue->toString(ss);
}

bool Z80::word::tryParse(ParsingContext* c)
{
    return c->expression(mValue, &RegisterNames, &ConditionNames, false);
}

bool Z80::word::canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const
{
    return mValue->canEvaluateValue(nextAddress, resolveError);
}

int Z80::word::low(int64_t currentAddress, int& high) const
{
    auto word = mValue->evaluateWord(&currentAddress);
    high = uint8_t((word >> 8) & 0xff);
    return uint8_t(word & 0xff);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::memBC::toString(std::stringstream& ss) const
{
    ss << "(BC)";
}

bool Z80::memBC::tryParse(ParsingContext* c)
{
    return c->consumeIdentifierInParentheses("bc");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::memDE::toString(std::stringstream& ss) const
{
    ss << "(DE)";
}

bool Z80::memDE::tryParse(ParsingContext* c)
{
    return c->consumeIdentifierInParentheses("de");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::memHL::toString(std::stringstream& ss) const
{
    ss << "(HL)";
}

bool Z80::memHL::tryParse(ParsingContext* c)
{
    return c->consumeIdentifierInParentheses("hl");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::memIX::toString(std::stringstream& ss) const
{
    ss << "(IX)";
}

bool Z80::memIX::tryParse(ParsingContext* c)
{
    return c->consumeIdentifierInParentheses("ix");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::memIY::toString(std::stringstream& ss) const
{
    ss << "(IY)";
}

bool Z80::memIY::tryParse(ParsingContext* c)
{
    return c->consumeIdentifierInParentheses("iy");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::memSP::toString(std::stringstream& ss) const
{
    ss << "(SP)";
}

bool Z80::memSP::tryParse(ParsingContext* c)
{
    return c->consumeIdentifierInParentheses("sp");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::memAddr::toString(std::stringstream& ss) const
{
    ss << '(';
    mValue->toString(ss);
    ss << ')';
}

bool Z80::memAddr::tryParse(ParsingContext* c)
{
    return c->expressionInParentheses(mValue, &RegisterNames, &ConditionNames, false);
}

bool Z80::memAddr::canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const
{
    return mValue->canEvaluateValue(nextAddress, resolveError);
}

int Z80::memAddr::low(int64_t currentAddress, int& high) const
{
    auto word = mValue->evaluateWord(&currentAddress);
    high = uint8_t((word >> 8) & 0xff);
    return uint8_t(word & 0xff);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::IX_byte::toString(std::stringstream& ss) const
{
    ss << "(IX";
    if (!mValue->isNegate()) {
        ss << '+';
        mValue->toString(ss);
    } else {
        ss << '-';
        static_cast<ExprNegate*>(mValue)->operand()->toString(ss);
    }
}

bool Z80::IX_byte::tryParse(ParsingContext* c)
{
    if (!c->consumeLeftParenthesis())
        return false;

    SourceLocation* registerLocation = c->token()->location();
    if (!c->consumeIdentifier("ix"))
        return false;

    if (c->token()->id() == TOK_PLUS) {
        c->nextToken();
        if (!c->expression(mValue, &RegisterNames, &ConditionNames, true))
            return false;
    } else if (c->token()->id() == TOK_MINUS) {
        SourceLocation* minusLocation = c->token()->location();
        c->nextToken();
        if (!c->expression(mValue, &RegisterNames, &ConditionNames, true))
            return false;
        mValue = new (c->heap()) ExprNegate(minusLocation, mValue);
    } else
        mValue = new (c->heap()) ExprNumber(registerLocation, 0);

    return c->consumeRightParenthesis();
}

bool Z80::IX_byte::canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const
{
    return mValue->canEvaluateValue(nextAddress, resolveError);
}

int Z80::IX_byte::value(int64_t currentAddress) const
{
    return mValue->evaluateByte(&currentAddress);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::IY_byte::toString(std::stringstream& ss) const
{
    ss << "(IY";
    if (!mValue->isNegate()) {
        ss << '+';
        mValue->toString(ss);
    } else {
        ss << '-';
        static_cast<ExprNegate*>(mValue)->operand()->toString(ss);
    }
}

bool Z80::IY_byte::tryParse(ParsingContext* c)
{
    if (!c->consumeLeftParenthesis())
        return false;

    SourceLocation* registerLocation = c->token()->location();
    if (!c->consumeIdentifier("iy"))
        return false;

    if (c->token()->id() == TOK_PLUS) {
        c->nextToken();
        if (!c->expression(mValue, &RegisterNames, &ConditionNames, true))
            return false;
    }
    else if (c->token()->id() == TOK_MINUS) {
        SourceLocation* minusLocation = c->token()->location();
        c->nextToken();
        if (!c->expression(mValue, &RegisterNames, &ConditionNames, true))
            return false;
        mValue = new (c->heap()) ExprNegate(minusLocation, mValue);
    }
    else
        mValue = new (c->heap()) ExprNumber(registerLocation, 0);

    return c->consumeRightParenthesis();
}

bool Z80::IY_byte::canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const
{
    return mValue->canEvaluateValue(nextAddress, resolveError);
}

int Z80::IY_byte::value(int64_t currentAddress) const
{
    return mValue->evaluateByte(&currentAddress);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::relOffset::toString(std::stringstream& ss) const
{
    mValue->toString(ss);
}

bool Z80::relOffset::tryParse(ParsingContext* c)
{
    return c->expression(mValue, &RegisterNames, &ConditionNames, false);
}

bool Z80::relOffset::canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const
{
    return mValue->canEvaluateValue(nextAddress, resolveError);
}

int Z80::relOffset::value(int64_t currentAddress, int64_t nextAddress) const
{
    return mValue->evaluateByteOffset(nextAddress, &currentAddress);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::portC::toString(std::stringstream& ss) const
{
    ss << "(C)";
}

bool Z80::portC::tryParse(ParsingContext* c)
{
    return c->consumeIdentifierInParentheses("c");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::portAddr::toString(std::stringstream& ss) const
{
    ss << '(';
    mValue->toString(ss);
    ss << ')';
}

bool Z80::portAddr::tryParse(ParsingContext* c)
{
    return c->expressionInParentheses(mValue, &RegisterNames, &ConditionNames, false);
}

bool Z80::portAddr::canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const
{
    return mValue->canEvaluateValue(nextAddress, resolveError);
}

int Z80::portAddr::value(int64_t currentAddress) const
{
    return mValue->evaluateByte(&currentAddress);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::intMode::toString(std::stringstream& ss) const
{
    mValue->toString(ss);
}

bool Z80::intMode::tryParse(ParsingContext* c)
{
    return c->expression(mValue, &RegisterNames, &ConditionNames, false);
}

bool Z80::intMode::canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const
{
    return mValue->canEvaluateValue(nextAddress, resolveError);
}

int Z80::intMode::value(int64_t currentAddress) const
{
    Value value = mValue->evaluateValue(&currentAddress);
    switch (value.number) {
        case 0: return 0x46;
        case 1: return 0x56;
        case 2: return 0x5E;
    }
    throw CompilerError(mValue->location(), "invalid operand for IM instruction.");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::rstIndex::toString(std::stringstream& ss) const
{
    mValue->toString(ss);
}

bool Z80::rstIndex::tryParse(ParsingContext* c)
{
    return c->expression(mValue, &RegisterNames, &ConditionNames, false);
}

bool Z80::rstIndex::canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const
{
    return mValue->canEvaluateValue(nextAddress, resolveError);
}

int Z80::rstIndex::value(int64_t currentAddress, uint8_t baseByte) const
{
    Value value = mValue->evaluateValue(&currentAddress);
    switch (value.number) {
        case 0x00:
        case 0x08:
        case 0x10:
        case 0x18:
        case 0x20:
        case 0x28:
        case 0x30:
        case 0x38:
            return uint8_t(baseByte | uint8_t(value.number));
    }
    throw CompilerError(mValue->location(), "invalid operand for RST instruction.");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::AF_::toString(std::stringstream& ss)
{
    ss << "AF'";
}

bool Z80::AF_::tryParse(ParsingContext* context)
{
    return context->consumeIdentifier("AF'");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define Z80_OPCODE_0(OP, BYTES, TSTATES) \
    static const auto OP##_bytes = toArray BYTES; \
    constexpr size_t Z80::OP::arraySizeInBytes() \
    { \
        int high = 0; (void)high; \
        int64_t nextAddress = 0; (void)nextAddress; \
        return OP##_bytes.size(); \
    } \
    bool Z80::OP::calculateSizeInBytes(size_t& outSize, std::unique_ptr<CompilerError>&) const \
    { \
        int high; (void)high; \
        int64_t nextAddress = 0; (void)nextAddress; \
        outSize = OP##_bytes.size(); \
        return true; \
    } \
    bool Z80::OP::canEmitCodeWithoutBaseAddress() const \
    { \
        return true; \
    } \
    bool Z80::OP::emitCode(CodeEmitter* emitter, int64_t& nextAddress, std::unique_ptr<CompilerError>&) const \
    { \
        int high; (void)high;\
        (void)nextAddress; \
        emitter->emitBytes(location(), OP##_bytes.data(), OP##_bytes.size()); \
        nextAddress += OP##_bytes.size(); \
        return true; \
    }

#define Z80_OPCODE_1(OP, OP1, BYTES, TSTATES) \
    constexpr size_t Z80::OP##_##OP1::arraySizeInBytes() \
    { \
        int high = 0; (void)high; \
        int64_t nextAddress = 0; (void)nextAddress; \
        return decltype(arrayType BYTES)::Size; \
    } \
    bool Z80::OP##_##OP1::calculateSizeInBytes(size_t& outSize, std::unique_ptr<CompilerError>&) const \
    { \
        int high; (void)high; \
        int64_t nextAddress = 0; (void)nextAddress; \
        outSize = decltype(arrayType BYTES)::Size; \
        return true; \
    } \
    bool Z80::OP##_##OP1::canEmitCodeWithoutBaseAddress() const \
    { \
        std::unique_ptr<CompilerError> resolveError; \
        return mOp1.canEvaluate(nullptr, resolveError); \
    } \
    bool Z80::OP##_##OP1::emitCode(CodeEmitter* emitter, int64_t& nextAddress, \
        std::unique_ptr<CompilerError>& resolveError) const \
    { \
        int high; (void)high;\
        (void)nextAddress; \
        if (!mOp1.canEvaluate(&nextAddress, resolveError)) \
            return false; \
        auto array = toArray BYTES; \
        emitter->emitBytes(location(), array.data(), array.size()); \
        nextAddress += array.size(); \
        return true; \
    }

#define Z80_OPCODE_2(OP, OP1, OP2, BYTES, TSTATES) \
    constexpr size_t Z80::OP##_##OP1##_##OP2::arraySizeInBytes() \
    { \
        int high = 0; (void)high; \
        int64_t nextAddress = 0; (void)nextAddress; \
        return decltype(arrayType BYTES)::Size; \
    } \
    bool Z80::OP##_##OP1##_##OP2::calculateSizeInBytes(size_t& outSize, std::unique_ptr<CompilerError>&) const \
    { \
        int high; (void)high; \
        int64_t nextAddress = 0; (void)nextAddress; \
        outSize = decltype(arrayType BYTES)::Size; \
        return true; \
    } \
    bool Z80::OP##_##OP1##_##OP2::canEmitCodeWithoutBaseAddress() const \
    { \
        std::unique_ptr<CompilerError> resolveError; \
        return mOp1.canEvaluate(nullptr, resolveError) && mOp2.canEvaluate(nullptr, resolveError); \
    } \
    bool Z80::OP##_##OP1##_##OP2::emitCode(CodeEmitter* emitter, int64_t& nextAddress, \
        std::unique_ptr<CompilerError>& resolveError) const \
    { \
        int high; (void)high;\
        (void)nextAddress; \
        if (!mOp1.canEvaluate(&nextAddress, resolveError) || !mOp2.canEvaluate(&nextAddress, resolveError)) \
            return false; \
        auto array = toArray BYTES; \
        emitter->emitBytes(location(), array.data(), array.size()); \
        nextAddress += array.size(); \
        return true; \
    }

#define OP1 mOp1.value(nextAddress)
#define OP2 mOp2.value(nextAddress)

#define OP1V(X) mOp1.value(nextAddress, (X))
#define OP2V(X) mOp2.value(nextAddress, (X))

#define OP1W mOp1.low(nextAddress, high), high
#define OP2W mOp2.low(nextAddress, high), high

#define NEXT (nextAddress + arraySizeInBytes())

#include "Instructions.Z80.hh"
