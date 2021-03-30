#include "Tests/Common.h"

TEST_CASE("data byte definition", "[data]")
{
    static const char source[] =
        "#section main\n"
        "db 0x01\n"
        "label1 db 0x02\n"
        "@@1 db 0x03\n"
        "@@2: db 0x04\n"
        "label2: db 5\n"
        "defb 6,7\n"
        "label3 db 8,0xff\n"
        "label4: defb 9,10\n"
        "@@5 db -0x80\n"
        "ld hl,label1\n"
        "ld hl,label1@@1\n"
        "ld hl,label1@@2\n"
        "ld hl,@@5\n"
        "ld hl,label3\n"
        "ld hl,label4\n"
        "db -1,-2,-3\n"
        ;

    static const unsigned char binary[] = {
        0x01,
        0x02,
        0x03,
        0x04,
        0x05,
        0x06,
        0x07,
        0x08,
        0xff,
        0x09,
        0x0a,
        0x80,
        0x21,
        0x35,
        0x12,
        0x21,
        0x36,
        0x12,
        0x21,
        0x37,
        0x12,
        0x21,
        0x3f,
        0x12,
        0x21,
        0x3b,
        0x12,
        0x21,
        0x3d,
        0x12,
        0xff,
        0xfe,
        0xfd,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("data byte overflow", "[data]")
{
    static const char source[] =
        "#section main\n"
        "db 0x100\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: value 256 (0x100) does not fit into a byte.");
}

TEST_CASE("data byte underflow", "[data]")
{
    static const char source[] =
        "#section main\n"
        "db -0x80-1\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: value -129 (0xffffffffffffff7f) does not fit into a byte.");
}

TEST_CASE("data word definition", "[data]")
{
    static const char source[] =
        "#section main\n"
        "dw 0xac01\n"
        "label1 dw 0xbe02\n"
        "@@1 defw 0xf103\n"
        "@@2: dw 0xe304\n"
        "label2: dw 5\n"
        "defw 6,7\n"
        "label3 dw 8,0xffff\n"
        "label4: dw 9,10\n"
        "@@5 dw -0x8000\n"
        "ld hl,label1\n"
        "ld hl,label1@@1\n"
        "ld hl,label1@@2\n"
        "ld hl,@@5\n"
        "ld hl,label3\n"
        "ld hl,label4\n"
        "dw -1,-2,-3\n"
        ;

    static const unsigned char binary[] = {
        0x01,
        0xac,
        0x02,
        0xbe,
        0x03,
        0xf1,
        0x04,
        0xe3,
        0x05,
        0x00,
        0x06,
        0x00,
        0x07,
        0x00,
        0x08,
        0x00,
        0xff,
        0xff,
        0x09,
        0x00,
        0x0a,
        0x00,
        0x00,
        0x80,
        0x21,
        0x36,
        0x12,
        0x21,
        0x38,
        0x12,
        0x21,
        0x3a,
        0x12,
        0x21,
        0x4a,
        0x12,
        0x21,
        0x42,
        0x12,
        0x21,
        0x46,
        0x12,
        0xff,
        0xff,
        0xfe,
        0xff,
        0xfd,
        0xff,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("data word overflow", "[data]")
{
    static const char source[] =
        "#section main\n"
        "dw 0x10000\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: value 65536 (0x10000) does not fit into a word.");
}

TEST_CASE("data word underflow", "[data]")
{
    static const char source[] =
        "#section main\n"
        "dw -0x8000-1\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: value -32769 (0xffffffffffff7fff) does not fit into a word.");
}

TEST_CASE("data dword definition", "[data]")
{
    static const char source[] =
        "#section main\n"
        "dd 0xbabeac01\n"
        "label1 dd 0xcafebe02\n"
        "@@1 dd 0xf103\n"
        "@@2: dd 0xe304\n"
        "label2: dd 5\n"
        "dd 6,7\n"
        "label3 dd 8,0xffffffff\n"
        "label4: dd 9,10\n"
        "@@5 dd -0x80000000\n"
        "ld hl,label1\n"
        "ld hl,label1@@1\n"
        "ld hl,label1@@2\n"
        "ld hl,@@5\n"
        "ld hl,label3\n"
        "ld hl,label4\n"
        "dd -1,-2,-3\n"
        ;

    static const unsigned char binary[] = {
        0x01,
        0xac,
        0xbe,
        0xba,
        0x02,
        0xbe,
        0xfe,
        0xca,
        0x03,
        0xf1,
        0x00,
        0x00,
        0x04,
        0xe3,
        0x00,
        0x00,
        0x05,
        0x00,
        0x00,
        0x00,
        0x06,
        0x00,
        0x00,
        0x00,
        0x07,
        0x00,
        0x00,
        0x00,
        0x08,
        0x00,
        0x00,
        0x00,
        0xff,
        0xff,
        0xff,
        0xff,
        0x09,
        0x00,
        0x00,
        0x00,
        0x0a,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x80,
        0x21,
        0x38,
        0x12,
        0x21,
        0x3c,
        0x12,
        0x21,
        0x40,
        0x12,
        0x21,
        0x60,
        0x12,
        0x21,
        0x50,
        0x12,
        0x21,
        0x58,
        0x12,
        0xff,
        0xff,
        0xff,
        0xff,
        0xfe,
        0xff,
        0xff,
        0xff,
        0xfd,
        0xff,
        0xff,
        0xff,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("data dword overflow", "[data]")
{
    static const char source[] =
        "#section main\n"
        "dd 0xffffffff+1\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: value 4294967296 (0x100000000) does not fit into a dword.");
}

TEST_CASE("data dword underflow", "[data]")
{
    static const char source[] =
        "#section main\n"
        "dd -0x80000000-1\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: value -2147483649 (0xffffffff7fffffff) does not fit into a dword.");
}

TEST_CASE("'$' in data definition", "[data]")
{
    static const char source[] =
        "#section main_0x15\n"
        "db $\n"
        "dw $\n"
        "dd $\n"
        "db $+1\n"
        "dw $+2\n"
        "dd $+3\n"
        "db $-1\n"
        "dw $-2\n"
        "dd $-3\n"
        ;

    static const unsigned char binary[] = {
        0x15,
        0x16,
        0x00,
        0x18,
        0x00,
        0x00,
        0x00,
        0x1d,
        0x1f,
        0x00,
        0x22,
        0x00,
        0x00,
        0x00,
        0x22,
        0x22,
        0x00,
        0x23,
        0x00,
        0x00,
        0x00,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("character and string literals", "[data]")
{
    static const char source[] =
        "#section main_0x15\n"
        "db 'x'\n"
        "db 'y',\"\",\"z\"\n"
        "db \"Hello, world!\"\n"
        "db \"\\n\\r\\\"'\",\"\\t\\f\\v\""
        ;

    static const unsigned char binary[] = {
        0x78,
        0x79,
        0x7A,
        0x48,
        0x65,
        0x6C,
        0x6C,
        0x6F,
        0x2C,
        0x20,
        0x77,
        0x6F,
        0x72,
        0x6C,
        0x64,
        0x21,
        0x0A,
        0x0D,
        0x22,
        0x27,
        0x09,
        0x0C,
        0x0B,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}
