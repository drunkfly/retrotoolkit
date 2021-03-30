#include "Tests/Common.h"

TEST_CASE("equ", "[equ]")
{
    static const char source[] =
        "#section main_0x100\n"
        "x equ 0x1234\n"
        "y equ 0x4322\n"
        "ld hl, x\n"
        "ld hl, y\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x34,
        0x12,
        0x21,
        0x22,
        0x43,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("referencing other equ", "[equ]")
{
    static const char source[] =
        "#section main_0x100\n"
        "x equ 0x1234\n"
        "y equ x + 0x4320\n"
        "ld hl, x\n"
        "ld hl, y\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x34,
        0x12,
        0x21,
        0x54,
        0x55,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("duplicate equ", "[equ]")
{
    static const char source[] =
        "#section main_0x100\n"
        "x equ 0x1234\n"
        "x equ 0x1234\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: duplicate identifier \"x\".");
}

TEST_CASE("local equ", "[equ]")
{
    static const char source[] =
        "#section main_0x100\n"
        "nonlocal1:\n"
        "@@1 equ 0x1234\n"
        "ld hl, @@1\n"
        "nonlocal2:\n"
        "@@1 equ 0x4321\n"
        "ld hl, @@1\n"
        "nonlocal3:\n"
        "@@1 equ 0xbabe\n"
        "ld hl, nonlocal1@@1\n"
        "ld hl, nonlocal2@@1\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x34,
        0x12,
        0x21,
        0x21,
        0x43,
        0x21,
        0x34,
        0x12,
        0x21,
        0x21,
        0x43,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("duplicate local equ", "[equ]")
{
    static const char source[] =
        "#section main_0x100\n"
        "nonlocal:\n"
        "@@1 equ 4\n"
        "@@1 equ 4\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:4: duplicate identifier \"nonlocal@@1\".");
}

TEST_CASE("duplicate full local equ", "[equ]")
{
    static const char source[] =
        "#section main_0x100\n"
        "nonlocal:\n"
        "@@1 equ 12\n"
        "nonlocal@@1 equ 12\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:4: duplicate identifier \"nonlocal@@1\".");
}

TEST_CASE("local equ without global", "[equ]")
{
    static const char source[] =
        "#section main_0x100\n"
        "@@1 equ 4\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: local label name without preceding global label.");
}

TEST_CASE("equ arithmetics", "[equ]")
{
    static const char source[] =
        "#section main_0x100\n"
        "x equ 4\n"
        "ld hl, x+1\n"
        "ld hl, x-3+1\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x05,
        0x00,
        0x21,
        0x02,
        0x00,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("conflicting equ name with label", "[equ]")
{
    static const char source[] =
        "#section main_0x100\n"
        "x equ 12\n"
        "x: ld hl, 4\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: duplicate identifier \"x\".");
}

TEST_CASE("conflicting label name with equ", "[equ]")
{
    static const char source[] =
        "#section main_0x100\n"
        "x: ld hl, 4\n"
        "x equ 12\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: duplicate identifier \"x\".");
}

TEST_CASE("missing expression after equ", "[equ]")
{
    static const char source[] =
        "#section main_0x100\n"
        "x equ ?\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: expected expression.");
}

TEST_CASE("reference equ between multiple files", "[equ]")
{
    static const char source1[] =
        "#section main\n"
        "equ1 equ 4\n"
        "ld a, (equ1)\n"
        "ld a, (equ2)\n"
        ;

    static const char source2[] =
        "#section main\n"
        "equ2 equ 8\n"
        "ld a, (equ2)\n"
        "ld a, (equ1)\n"
        ;

    static const unsigned char binary[] = {
        0x3a,
        0x04,
        0x00,
        0x3a,
        0x08,
        0x00,
        0x3a,
        0x08,
        0x00,
        0x3a,
        0x04,
        0x00,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble2(errorConsumer, source1, source2);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("labels in equ", "[equ]")
{
    static const char source1[] =
        "#section sec1\n"
        "equ1 equ label1\n"
        "equ2 equ label2+1\n"
        "label1:"
        "ld a, (equ1)\n"
        "ld a, (equ2)\n"
        "ld a, (equ3)\n"
        "ld a, (equ4)\n"
        ;

    static const char source2[] =
        "#section sec2\n"
        "equ3 equ label1\n"
        "equ4 equ label2\n"
        "label2:"
        "ld a, (equ1)\n"
        "ld a, (equ2)\n"
        "ld a, (equ3)\n"
        "ld a, (equ4)\n"
        ;

    static const unsigned char binary[] = {
        0x3a,
        0x30,
        0x12,
        0x3a,
        0x3d,
        0x12,
        0x3a,
        0x30,
        0x12,
        0x3a,
        0x3c,
        0x12,
        0x3a,
        0x30,
        0x12,
        0x3a,
        0x3d,
        0x12,
        0x3a,
        0x30,
        0x12,
        0x3a,
        0x3c,
        0x12,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble2(errorConsumer, source1, source2);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("circular reference", "[equ]")
{
    static const char source[] =
        "#section main_0x100\n"
        "x equ y\n"
        "y equ x\n"
        "db 0\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE((errorConsumer.errorMessage() == "source:2: hit circular dependency while evaluating expression." ||
             errorConsumer.errorMessage() == "source:3: hit circular dependency while evaluating expression."));
}

TEST_CASE("deeper circular reference", "[equ]")
{
    static const char source[] =
        "#section main_0x100\n"
        "x equ y\n"
        "y equ z+4\n"
        "z equ 12-w+5\n"
        "w equ 3+x\n"
        "db 0\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE((errorConsumer.errorMessage() == "source:2: hit circular dependency while evaluating expression." ||
             errorConsumer.errorMessage() == "source:3: hit circular dependency while evaluating expression." ||
             errorConsumer.errorMessage() == "source:4: hit circular dependency while evaluating expression." ||
             errorConsumer.errorMessage() == "source:5: hit circular dependency while evaluating expression."));
}

TEST_CASE("'$' in equ", "[equ]")
{
    static const char source[] =
        "#section sec1_0x1234\n"
        "equ1 equ $\n"
        "equ2 equ $-2\n"
        "ld a, (equ1)\n"
        "ld a, (equ3)\n"
        "ld a, (equ1)\n"
        "equ3 equ $\n"
        "ld a, (equ2)\n"
        ;

    static const unsigned char binary[] = {
        0x3a,
        0x34,
        0x12,
        0x3a,
        0x3d,
        0x12,
        0x3a,
        0x34,
        0x12,
        0x3a,
        0x32,
        0x12,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("'$' outside of a section", "[equ]")
{
    static const char source[] =
        "equ1 equ 3*(4+$)\n"
        "#section main\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:1: current address is not available outside of a section.");
}

TEST_CASE("ensure unused equs are validated", "[equ]")
{
    static const char source[] =
        "#section main_0x100\n"
        "x equ y+4\n"
        "db 0\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: use of undeclared identifier 'y'.");
}

TEST_CASE("equ in bit instruction", "[equ]")
{
    static const char source[] =
        "#section main\n"
        "mybit equ 4-3\n"
        "offs equ (40+3)\n"
        "bit mybit, (ix-offs)\n"
        ;

    static const unsigned char binary[] = {
        0xdd,
        0xcb,
        0xd5,
        0x4e,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("invalid equ in bit instruction", "[equ]")
{
    static const char source[] =
        "#section main\n"
        "mybit equ 2*4\n"
        "offs equ (40+3)\n"
        "bit mybit, (ix-offs)\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:4: bit index is out of range.");
}

TEST_CASE("equ in rst instruction", "[equ]")
{
    static const char source[] =
        "#section main\n"
        "addr equ 10+6\n"
        "rst addr\n"
        ;

    static const unsigned char binary[] = {
        0xd7,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("invalid equ in rst instruction", "[equ]")
{
    static const char source[] =
        "#section main\n"
        "addr equ 128-4\n"
        "rst addr\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: invalid operand for RST instruction.");
}
