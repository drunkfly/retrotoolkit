#include "Value.h"

Value::Value()
    : number(0)
    , sign(Sign::Unsigned)
    , bits(SignificantBits::NoMoreThan8)
{
}

Value::Value(int64_t n)
    : number(n)
    , sign(n < 0 ? Sign::Signed : Sign::Unsigned)
    , bits(significantBitsForNumber(n))
{
}

Value::Value(int64_t n, Sign sign)
    : number(n)
    , sign(sign)
    , bits(significantBitsForNumber(n))
{
}

Value::Value(int64_t n, Sign sign, SignificantBits b)
    : number(n)
    , sign(sign)
    , bits(b)
{
}

void Value::truncateTo8Bit()
{
    if (sign == Sign::Signed)
        number = int64_t(int8_t(number & 0xff));
    else
        number = int64_t(uint8_t(number & 0xff));
}

void Value::truncateTo16Bit()
{
    if (sign == Sign::Signed)
        number = int64_t(int16_t(number & 0xffff));
    else
        number = int64_t(uint16_t(number & 0xffff));
}

void Value::truncateTo32Bit()
{
    if (sign == Sign::Signed)
        number = int64_t(int32_t(number & 0xffffffff));
    else
        number = int64_t(uint32_t(number & 0xffffffff));
}

void Value::truncateToSignificantBits()
{
    switch (bits) {
        case SignificantBits::NoMoreThan8: truncateTo8Bit(); return;
        case SignificantBits::NoMoreThan16: truncateTo16Bit(); return;
        case SignificantBits::All: return;
    }
}

SignificantBits Value::significantBitsForNumber(int64_t n)
{
    if (n >= -128 && n < 256)
        return SignificantBits::NoMoreThan8;
    else if (n >= -32768 && n < 65536)
        return SignificantBits::NoMoreThan16;
    else
        return SignificantBits::All;
}

SignificantBits Value::significantBitsForNumber16(int64_t n)
{
    if (n >= -32768 && n < 65536)
        return SignificantBits::NoMoreThan16;
    else
        return SignificantBits::All;
}
