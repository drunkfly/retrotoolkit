#include "Tests/Common.h"

TEST_CASE("label at end of file", "[labels]")
{
    static const char source[] =
        "#section main\n"
        "y:\n"
        "dw @@x\n"
        "@@x:\n"
        ;

    static const unsigned char binary[] = {
        0x36,
        0x12,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("forward label", "[labels]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, fwdlabel\n"
        "fwdlabel:\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x03,
        0x01,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("backward label", "[labels]")
{
    static const char source[] =
        "#section main_0x100\n"
        "backlabel:\n"
        "ld hl, backlabel\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x00,
        0x01,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("duplicate label", "[labels]")
{
    static const char source[] =
        "#section main_0x100\n"
        "label1: nop\n"
        "label1: nop\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: duplicate identifier \"label1\".");
}

TEST_CASE("local labels", "[labels]")
{
    static const char source[] =
        "#section main_0x100\n"
        "test@@1:\n"
        "nonlocal1:\n"
        "@@1: ld hl, @@1\n"
        "ld hl, nonlocal1@@1\n"
        "ld hl, nonlocal2@@1\n"
        "nonlocal2:\n"
        "@@1: ld hl, @@1\n"
        "ld hl, nonlocal1@@1\n"
        "ld hl, nonlocal2@@1\n"
        "ld hl, test@@1\n"
        "nonlocal2@@2:\n"
        "test@@3:\n"
        "ld hl, test@@3\n"
        "ld hl, nonlocal2@@2\n"
        "ld hl, @@1\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x00,
        0x01,
        0x21,
        0x00,
        0x01,
        0x21,
        0x09,
        0x01,
        0x21,
        0x09,
        0x01,
        0x21,
        0x00,
        0x01,
        0x21,
        0x09,
        0x01,
        0x21,
        0x00,
        0x01,
        0x21,
        0x15,
        0x01,
        0x21,
        0x15,
        0x01,
        0x21,
        0x09,
        0x01,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("duplicate local label", "[labels]")
{
    static const char source[] =
        "#section main_0x100\n"
        "nonlocal: nop\n"
        "@@1: nop\n"
        "@@1: nop\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:4: duplicate identifier \"nonlocal@@1\".");
}

TEST_CASE("duplicate full local label", "[labels]")
{
    static const char source[] =
        "#section main_0x100\n"
        "nonlocal: nop\n"
        "@@1: nop\n"
        "nonlocal@@1: nop\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:4: duplicate identifier \"nonlocal@@1\".");
}

TEST_CASE("local without global 1", "[labels]")
{
    static const char source[] =
        "#section main_0x100\n"
        "@@1: nop\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: local label name without preceding global label.");
}

TEST_CASE("local without global 2", "[labels]")
{
    static const char source[] =
        "#section main_0x100\n"
        "test@@1:\n"
        "@@1: nop\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: local label name without preceding global label.");
}

TEST_CASE("label arithmetics", "[labels]")
{
    static const char source[] =
        "#section main_0x100\n"
        "ld hl, fwdlabel+1\n"
        "ld hl, fwdlabel-3+1\n"
        "fwdlabel:\n"
        ;

    static const unsigned char binary[] = {
        0x21,
        0x07,
        0x01,
        0x21,
        0x04,
        0x01,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("orphan local label", "[labels]")
{
    static const char source[] =
        "#section main_0x100\n"
        "nonlocal: nop\n"
        "@@1\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: missing ':' after local label name.");
}

TEST_CASE("ld a, (label)", "[labels]")
{
    static const char source[] =
        "#section main\n"
        "label:\n"
        "ld a, (label)\n"
        "ld a, (label+4)\n"
        "ld a, (label-(1+1))\n"
        ;

    static const unsigned char binary[] = {
        0x3a,
        0x34,
        0x12,
        0x3a,
        0x38,
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

TEST_CASE("reference labels between multiple files", "[labels]")
{
    static const char source1[] =
        "#section sec1\n"
        "label1:\n"
        "ld a, (label1)\n"
        "ld a, (label2)\n"
        ;

    static const char source2[] =
        "#section sec2_0x1236\n"
        "label2:\n"
        "ld a, (label2)\n"
        "ld a, (label1)\n"
        ;

    static const unsigned char binary[] = {
        0x3a,
        0x30,
        0x12,
        0x3a,
        0x36,
        0x12,
        0x3a,
        0x36,
        0x12,
        0x3a,
        0x30,
        0x12,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble2(errorConsumer, source1, source2);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("label at 64K", "[labels]")
{
    static const char source[] =
        "#section main_0xffff\n"
        "label1: db 0xfd\n"
        ;

    static const unsigned char binary[] = {
        0xfd,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("label over 64K 1", "[labels]")
{
    static const char source[] =
        "#section main_0xfffe\n"
        "label1: dw label1\n"
        "label2:\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: address is over 64K.");
}

TEST_CASE("label over 64K 2", "[labels]")
{
    static const char source[] =
        "#section main_0xffff\n"
        "label1: dw label1\n"
        "label2:\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: address is over 64K.");
}
