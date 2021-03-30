#include "Tests/Common.h"

TEST_CASE("addition", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, 5+0x10\n"
        "ld hl, 0x8000+(-0x8000)\n"
        "ld a, 0x80+(-0x80)\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x15,
        0x00,
        0x21,
        0x00,
        0x00,
        0x3e,
        0x00,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("8 bit addition overflow", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld a, 0x80+0x80\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: value 256 (0x100) does not fit into a byte.");
}

TEST_CASE("16 bit addition overflow", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, 0x8000+0x8000\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: value 65536 (0x10000) does not fit into a word.");
}

TEST_CASE("subtraction", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, 5-1\n"
        "ld hl, 5-5\n"
        "ld hl, 5-6\n"
        "ld hl, 10-8-1\n"
        "ld hl, 0x8000-0x8000\n"
        "ld a, 0x80-0x80\n"
        "ld hl, 0x10000-0x8000\n"
        "ld a, 0x100-0x80\n"
        "ld hl, 0x8000-0x10000\n"
        "ld a, 0x80-0x100\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x04,
        0x00,
        0x21,
        0x00,
        0x00,
        0x21,
        0xff,
        0xff,
        0x21,
        0x01,
        0x00,
        0x21,
        0x00,
        0x00,
        0x3e,
        0x00,
        0x21,
        0x00,
        0x80,
        0x3e,
        0x80,
        0x21,
        0x00,
        0x80,
        0x3e,
        0x80,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("8 bit subtraction overflow", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld a, 0x80-0x180\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: value -256 (0xffffffffffffff00) does not fit into a byte.");
}

TEST_CASE("16 bit subtraction overflow", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, 0x8000-0x18000\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: value -65536 (0xffffffffffff0000) does not fit into a word.");
}

TEST_CASE("multiplication", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, 3*5\n"
        "ld hl, 4*12*2\n"
        "ld hl, -2*3\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x0f,
        0x00,
        0x21,
        0x60,
        0x00,
        0x21,
        0xfa,
        0xff,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("division", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, 10/5\n"
        "ld hl, 11/3\n"
        "ld hl, 11/3/2\n"
        "ld hl, -5/2\n"
        "ld hl, 10%5\n"
        "ld hl, 11%3\n"
        "ld hl, 11/3%2\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x02,
        0x00,
        0x21,
        0x03,
        0x00,
        0x21,
        0x01,
        0x00,
        0x21,
        0xfe,
        0xff,
        0x21,
        0x00,
        0x00,
        0x21,
        0x02,
        0x00,
        0x21,
        0x01,
        0x00,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("negation", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, -0\n"
        "ld hl, -1\n"
        "ld hl, -0x8000\n"
        "ld hl, -(~0x7fff)\n"
        "ld a, -(~0x7f)\n"
        "ld a, -0x80\n"
        "ld a, -0\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x00,
        0x00,
        0x21,
        0xff,
        0xff,
        0x21,
        0x00,
        0x80,
        0x21,
        0x00,
        0x80,
        0x3e,
        0x80,
        0x3e,
        0x80,
        0x3e,
        0x00,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("8 bit negation overflow", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld a, -0x81\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: value -129 (0xffffffffffffff7f) does not fit into a byte.");
}

TEST_CASE("16 bit negation overflow", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, -0x8001\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: value -32769 (0xffffffffffff7fff) does not fit into a word.");
}

TEST_CASE("logical not", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, !0\n"
        "ld hl, !1\n"
        "ld hl, !(0-1)\n"
        "ld a, !(-0)\n"
        "ld a, !(-1)\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x01,
        0x00,
        0x21,
        0x00,
        0x00,
        0x21,
        0x00,
        0x00,
        0x3e,
        0x01,
        0x3e,
        0x00,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("bitwise not", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, ~0\n"
        "ld hl, ~1\n"
        "ld hl, ~0xffff\n"
        "ld hl, ~0xfefe\n"
        "ld hl, ~(0-1)\n"
        "ld a, ~0\n"
        "ld a, ~1\n"
        "ld a, ~0xff\n"
        "ld a, ~0xfe\n"
        "ld a, ~(0-1)\n"
        "ld a, +(~255 + 1) / 256\n"
        "ld a, +(~2)\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0xff,
        0xff,
        0x21,
        0xfe,
        0xff,
        0x21,
        0x00,
        0x00,
        0x21,
        0x01,
        0x01,
        0x21,
        0x00,
        0x00,
        0x3e,
        0xff,
        0x3e,
        0xfe,
        0x3e,
        0x00,
        0x3e,
        0x01,
        0x3e,
        0x00,
        0x3e,
        0xff,
        0x3e,
        0xfd,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("parentheses", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, +(4)\n"
        "ld hl, +(1-4)\n"
        "ld hl, 2*(5-3)\n"
        "ld hl, +(2*5)-3\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x04,
        0x00,
        0x21,
        0xFD,
        0xFF,
        0x21,
        0x04,
        0x00,
        0x21,
        0x07,
        0x00,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("shifts", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, 1<<0\n"
        "ld hl, 1<<1\n"
        "ld hl, 1<<15\n"
        "ld hl, 0<<16\n"
        "ld hl, 0xab<<8\n"
        "ld hl, 0x7fff<<1\n"
        "ld hl, 0x7fff<<1>>1\n"
        "ld a, 1<<0\n"
        "ld a, 1<<1\n"
        "ld a, 1<<7\n"
        "ld a, 0<<8\n"
        "ld a, 1<<1<<1\n"
        "ld a, 1<<1+2\n"
        "ld a, +(1<<1)+2\n"
        "ld a, 2<<~0xfc\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x01,
        0x00,
        0x21,
        0x02,
        0x00,
        0x21,
        0x00,
        0x80,
        0x21,
        0x00,
        0x00,
        0x21,
        0x00,
        0xab,
        0x21,
        0xfe,
        0xff,
        0x21,
        0xff,
        0x7f,
        0x3e,
        0x01,
        0x3e,
        0x02,
        0x3e,
        0x80,
        0x3e,
        0x00,
        0x3e,
        0x04,
        0x3e,
        0x08,
        0x3e,
        0x04,
        0x3e,
        0x10,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("8 bit left shift overflow", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld a, 1<<8\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: value 256 (0x100) does not fit into a byte.");
}

TEST_CASE("16 bit left shift overflow", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, 1<<16\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: value 65536 (0x10000) does not fit into a word.");
}

TEST_CASE("negative left shift counter", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld a, 1<<(7-8)\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: negative shift count for operator '<<'.");
}

TEST_CASE("negative right shift counter", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld a, 1>>(7-8)\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: negative shift count for operator '>>'.");
}

TEST_CASE("large left shift counter", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld a, 1<<65\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: shift count is too large for operator '<<'.");
}

TEST_CASE("large right shift counter", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld a, 1>>65\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: shift count is too large for operator '>>'.");
}

TEST_CASE("relational operators", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, 1==1\n"
        "ld hl, 1==2\n"
        "ld hl, 1!=1\n"
        "ld hl, 1!=2\n"
        "ld hl, 1<2\n"
        "ld hl, 2<1\n"
        "ld hl, -1<2\n"
        "ld a, -2<1\n"
        "ld a, 1<-2\n"
        "ld a, 2<-1\n"
        "ld a, 1<=2\n"
        "ld a, 2<=1\n"
        "ld a, 1<=1\n"
        "ld a, -1<=2\n"
        "ld a, -2<=1\n"
        "ld a, -1<=-1\n"
        "ld a, 1<1\n"
        "ld a, 1>1\n"
        "ld a, 1>2\n"
        "ld a, 2>1\n"
        "ld a, -1>-1\n"
        "ld a, -1>-2\n"
        "ld a, -2>-1\n"
        "ld a, -1>1\n"
        "ld a, -1>2\n"
        "ld a, -2>1\n"
        "ld a, 1>-1\n"
        "ld a, 1>-2\n"
        "ld a, 2>-1\n"
        "ld a, 1>=1\n"
        "ld a, 1>=2\n"
        "ld a, 2>=1\n"
        "ld a, -1>=-1\n"
        "ld a, -1>=-2\n"
        "ld a, -2>=-1\n"
        "ld a, -1>=1\n"
        "ld a, -1>=2\n"
        "ld a, -2>=1\n"
        "ld a, 1>=-1\n"
        "ld a, 1>=-2\n"
        "ld a, 2>=-1\n"
        "ld a, 2>1<3\n"
        "ld a, 2>1>=1\n"
        "ld a, 2<1>=1==0\n"
        "ld a, 2>1>=1==0\n"
        "ld a, 2<1>=1!=0\n"
        "ld a, 2>1>=1!=0\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x01,
        0x00,
        0x21,
        0x00,
        0x00,
        0x21,
        0x00,
        0x00,
        0x21,
        0x01,
        0x00,
        0x21,
        0x01,
        0x00,
        0x21,
        0x00,
        0x00,
        0x21,
        0x01,
        0x00,
        0x3e,
        0x01,
        0x3e,
        0x00,
        0x3e,
        0x00,
        0x3e,
        0x01,
        0x3e,
        0x00,
        0x3e,
        0x01,
        0x3e,
        0x01,
        0x3e,
        0x01,
        0x3e,
        0x01,
        0x3e,
        0x00,
        0x3e,
        0x00,
        0x3e,
        0x00,
        0x3e,
        0x01,
        0x3e,
        0x00,
        0x3e,
        0x01,
        0x3e,
        0x00,
        0x3e,
        0x00,
        0x3e,
        0x00,
        0x3e,
        0x00,
        0x3e,
        0x01,
        0x3e,
        0x01,
        0x3e,
        0x01,
        0x3e,
        0x01,
        0x3e,
        0x00,
        0x3e,
        0x01,
        0x3e,
        0x01,
        0x3e,
        0x01,
        0x3e,
        0x00,
        0x3e,
        0x00,
        0x3e,
        0x00,
        0x3e,
        0x00,
        0x3e,
        0x01,
        0x3e,
        0x01,
        0x3e,
        0x01,
        0x3e,
        0x01,
        0x3e,
        0x01,
        0x3e,
        0x01,
        0x3e,
        0x00,
        0x3e,
        0x00,
        0x3e,
        0x01,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("bitwise operators", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, 2|8\n"
        "ld hl, 0x2148|0x8421\n"
        "ld hl, 3&1\n"
        "ld hl, 1&3\n"
        "ld hl, 8|1&3\n"
        "ld hl, 1^1\n"
        "ld hl, 1^0\n"
        "ld a, 0^1\n"
        "ld a, 0xff^0xaa\n"
        "ld a, 5&~3\n"
        "ld a, ~3|1\n"
        "ld hl, ~3|1\n"
        "ld a, -1^1\n"
        "ld hl, -1^1\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x0a,
        0x00,
        0x21,
        0x69,
        0xa5,
        0x21,
        0x01,
        0x00,
        0x21,
        0x01,
        0x00,
        0x21,
        0x09,
        0x00,
        0x21,
        0x00,
        0x00,
        0x21,
        0x01,
        0x00,
        0x3e,
        0x01,
        0x3e,
        0x55,
        0x3e,
        0x04,
        0x3e,
        0xfd,
        0x21,
        0xfd,
        0xff,
        0x3e,
        0xfe,
        0x21,
        0xfe,
        0xff,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("logical operators", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, 1||1\n"
        "ld hl, 1||0\n"
        "ld hl, 0||0\n"
        "ld hl, 2&&1\n"
        "ld hl, 2&&0\n"
        "ld a, 1&&-3||0\n"
        "ld a, -4||-5&&3\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x01,
        0x00,
        0x21,
        0x01,
        0x00,
        0x21,
        0x00,
        0x00,
        0x21,
        0x01,
        0x00,
        0x21,
        0x00,
        0x00,
        0x3e,
        0x01,
        0x3e,
        0x01,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("conditional operator", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, +(0?4:12)\n"
        "ld hl, +(1?7:11)\n"
        "ld a, +(5<8?3:9)\n"
        "ld a, +(0>-3?15:2)\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x0c,
        0x00,
        0x21,
        0x07,
        0x00,
        0x3e,
        0x03,
        0x3e,
        0x0f,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}
