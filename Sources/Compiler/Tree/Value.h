#ifndef COMPILER_TREE_VALUE_H
#define COMPILER_TREE_VALUE_H

#include <stdint.h>

enum class Sign
{
    Signed,
    Unsigned,
};

enum class SignificantBits
{
    // note: order of constants is important
    NoMoreThan8 = 0,
    NoMoreThan16 = 1,
    All = 2,
};

struct Value
{
    int64_t number;
    Sign sign;
    SignificantBits bits;

    Value();
    explicit Value(int64_t n);
    Value(int64_t n, Sign sign);
    Value(int64_t n, Sign sign, SignificantBits b);

    void truncateTo8Bit();
    void truncateTo16Bit();
    void truncateTo32Bit();
    void truncateToSignificantBits();

    static SignificantBits significantBitsForNumber(int64_t n);
    static SignificantBits significantBitsForNumber16(int64_t n);
};

#endif
