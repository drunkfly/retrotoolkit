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

TEST_CASE("@here variable", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "label:\n"
        "ld a, {@here Var1}\n"
        "ld hl, {@here Var2=0x1234}\n"
        "ld de, ({@here @@Var3})\n"
        "jp {@here Var4=label@@Var5}\n"
        "ld a, (ix+{@here label@@Var5})\n"
        "ld b, (iy+{@here Var6=-1})\n"
        "ld (ix+{@here Var7=1}), {@here Var8}\n"
        "dw Var1\n"
        "dw Var2\n"
        "dw @@Var3\n"
        "dw Var4\n"
        "dw label@@Var5\n"
        "dw Var6\n"
        "dw Var7\n"
        "dw Var8\n"
        ;

    static const unsigned char binary[] = {
        0x3e, // 0x00   ld a, #
        0x00, // 0x01   Var1
        0x21, // 0x02   ld hl, ##
        0x34, // 0x03   Var2
        0x12, // 0x04
        0xed, // 0x05   ld de, (##)
        0x5b, // 0x06
        0x00, // 0x07   Var3
        0x00, // 0x08
        0xc3, // 0x09   jp ##
        0x0e, // 0x0a   Var4
        0x01, // 0x0b
        0xdd, // 0x0c   ld a, (ix+#)
        0x7e, // 0x0d
        0x00, // 0x0e   Var5
        0xfd, // 0x0f   ld b, (iy+#)
        0x46, // 0x10
        0xff, // 0x11   Var6
        0xdd, // 0x12   ld (ix+#), #
        0x36, // 0x13
        0x01, // 0x14   Var7
        0x00, // 0x15   Var8
        0x01, // 0x16   dw Var1
        0x01, // 0x17
        0x03, // 0x18   dw Var2
        0x01, // 0x19
        0x07, // 0x1a   dw Var3
        0x01, // 0x1b
        0x0a, // 0x1c   dw Var4
        0x01, // 0x1d
        0x0e, // 0x1e   dw Var5
        0x01, // 0x1f
        0x11, // 0x20   dw Var6
        0x01, // 0x21
        0x14, // 0x22   dw Var7
        0x01, // 0x23
        0x15, // 0x24   dw Var8
        0x01, // 0x25
    };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("duplicate @here variable 1", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld a, {@here v1}\n"
        "ld b, {@here v1}\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: duplicate identifier \"v1\".");
}

TEST_CASE("duplicate @here variable 2", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld (ix+{@here v}), {@here v}\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: duplicate identifier \"v\".");
}

TEST_CASE("invalid @here variable 1", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld a, {@here v1}+1\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: @here variable cannot be used in expression.");
}

TEST_CASE("invalid @here variable 2", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "bit {@here v1}, a\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: @here is not allowed in this context.");
}

TEST_CASE("invalid @here variable 3", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld a, (ix-{@here q})\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: @here should not be used with IX-# or IY-#. Use IX+# or IY+# instead.");
}

TEST_CASE("invalid @here variable 4", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "y equ {@here m}\n"
        "ld a, y\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: @here is not allowed in this context.");
}

TEST_CASE("invalid @here variable 5", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "rst {@here v1}\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: @here is not allowed in this context.");
}

TEST_CASE("invalid @here variable 6", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "im {@here v1}\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: @here is not allowed in this context.");
}

TEST_CASE("disallow @here variable as initializer", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld c, {@here v={@here q}}\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: @here variable cannot be used as initializer for another @here variable.");
}

TEST_CASE("local @here variable without global", "[expr]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld a, {@here @@v}\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: local label name without preceding global label.");
}
