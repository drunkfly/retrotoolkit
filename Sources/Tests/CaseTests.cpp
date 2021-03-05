#include <catch.hpp>
#include "Tests/Util/TestUtil.h"

TEST_CASE("case insensitivity in instructions and operands", "[case]")
{
    static const char source[] =
        "#SeCtIoN main\n"
        "x eQu 43\n"
        "y eqU 85\n"
        "Adc A, (Ix-x)\n"
        "aDc a, (iX+y)\n"
        "adD hL, Sp\n"
        "sEt 0, (HL)\n"
        "ex Af, aF'\n"
        "dB 0x01\n"
        "Dw 0xc0de\n"
        "DD 0xbadc0de\n"
        ;

    static const unsigned char binary[] = {
        0xdd,
        0x8e,
        0xd5,
        0xdd,
        0x8e,
        0x55,
        0x39,
        0xcb,
        0xc6,
        0x08,
        0x01,
        0xde,
        0xc0,
        0xde,
        0xc0,
        0xad,
        0x0b,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("case sensitivity in names", "[case]")
{
    static const char source[] =
        "#section main\n"
        "#section mAiN\n"
        "equ1 equ 1\n"
        "eQu1 equ 2\n"
        "label1:\n"
        "ld hl, label1\n"
        "lAbEl1:\n"
        "ld hl, lAbEl1\n"
        "ld hl, equ1\n"
        "ld hl, eQu1\n"
        "#repeat 2, cNt1\n"
        "db 0xff,cNt1\n"
        "#repeat 2, CnT1\n"
        "db 0x80,CnT1,cNt1\n"
        "#endrepeat\n"
        "#endrepeat\n"
        ;

    static const unsigned char binary[] = {
        0x00,
        0x21,
        0x35,
        0x12,
        0x21,
        0x38,
        0x12,
        0x21,
        0x01,
        0x00,
        0x21,
        0x02,
        0x00,
        0xff,
        0x00,
        0x80,
        0x00,
        0x00,
        0x80,
        0x01,
        0x00,
        0xff,
        0x01,
        0x80,
        0x00,
        0x01,
        0x80,
        0x01,
        0x01,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}
