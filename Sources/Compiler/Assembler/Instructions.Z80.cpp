#include "Instructions.Z80.h"
#include "Compiler/Token.h"

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::byte::toString(std::stringstream& ss) const
{
    mValue->toString(ss);
}

bool Z80::byte::tryParse(ParsingContext* c)
{
    return c->expression(mValue, &RegisterNames, &ConditionNames, false);
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
    ss << '('; mValue->toString(ss); ss << ')';
}

bool Z80::memAddr::tryParse(ParsingContext* c)
{
    return c->expressionInParentheses(mValue, &RegisterNames, &ConditionNames);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::IX_byte::toString(std::stringstream& ss) const
{
    ss << "(ix";
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
    if (!c->consumeIdentifier("ix"))
        return false;

    if (c->token()->id() == TOK_PLUS) {
        c->nextToken();
        if (!c->expression(mValue, &RegisterNames, &ConditionNames, true))
            return false;
    } else if (c->token()->id() == TOK_MINUS) {
        c->nextToken();
        if (!c->expression(mValue, &RegisterNames, &ConditionNames, true))
            return false;
        mValue = new (c->heap()) ExprNegate(mValue->location(), mValue);
    }

    return c->consumeRightParenthesis();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::IY_byte::toString(std::stringstream& ss) const
{
    ss << "(iy";
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
    if (!c->consumeIdentifier("iy"))
        return false;

    if (c->token()->id() == TOK_PLUS) {
        c->nextToken();
        if (!c->expression(mValue, &RegisterNames, &ConditionNames, true))
            return false;
    } else if (c->token()->id() == TOK_MINUS) {
        c->nextToken();
        if (!c->expression(mValue, &RegisterNames, &ConditionNames, true))
            return false;
        mValue = new (c->heap()) ExprNegate(mValue->location(), mValue);
    }

    return c->consumeRightParenthesis();
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
    return c->expressionInParentheses(mValue, &RegisterNames, &ConditionNames);
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Z80::rstIndex::toString(std::stringstream& ss) const
{
    mValue->toString(ss);
}

bool Z80::rstIndex::tryParse(ParsingContext* c)
{
    return c->expression(mValue, &RegisterNames, &ConditionNames, false);
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

#define Z80_OPCODE_0(OP, BYTES, TSTATES)
#define Z80_OPCODE_1(OP, OP1, BYTES, TSTATES)
#define Z80_OPCODE_2(OP, OP1, OP2, BYTES, TSTATES)

#include "Instructions.Z80.hh"
