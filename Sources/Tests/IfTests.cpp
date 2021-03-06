#include <catch.hpp>
#include "Tests/Util/TestUtil.h"

TEST_CASE("if", "[if]")
{
    static const char source[] =
        "#section main_0x100\n"
        "#if 0\n"
        "db 0x01\n"
        "#endif\n"
        "#if 1\n"
        "db 0x02\n"
        "#endif\n"
        "#if 0\n"
        "db 0x03\n"
        "#else\n"
        "db 0x04\n"
        "#endif\n"
        "#if 1\n"
        "db 0x05\n"
        "#else\n"
        "db 0x06\n"
        "#endif\n"
        "#if 0\n"
        "#if 1\n"
        "db 0x07\n"
        "#else\n"
        "db 0x08\n"
        "#endif\n"
        "#else\n"
        "#if 0\n"
        "db 0x09\n"
        "#else\n"
        "db 0x0a\n"
        "#endif\n"
        "#endif\n"
        "#if 1\n"
        "#if 0\n"
        "db 0x0b\n"
        "#else\n"
        "db 0x0c\n"
        "#endif\n"
        "#else\n"
        "#if 1\n"
        "db 0x0d\n"
        "#else\n"
        "db 0x0e\n"
        "#endif\n"
        "#endif\n"
        ;

    static const unsigned char binary[] = {
        0x02,
        0x04,
        0x05,
        0x0a,
        0x0c,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

/* FIXME
TEST_CASE("equ in if", "[if]")
{
    static const char source[] =
        "section main [base 0x100]\n"
        "if 1\n"
        "x equ 0xaa\n"
        "else\n"
        "x equ 0xcc\n"
        "endif\n"
        "if 0\n"
        "y equ 0xbb\n"
        "else\n"
        "y equ 0xdd\n"
        "endif\n"
        "db x,y\n"
        ;

    static const unsigned char binary[] = {
        0xaa,
        0xdd,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.lastErrorMessage() == "");
    REQUIRE(errorConsumer.errorCount() == 0);
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}
*/

TEST_CASE("equ with label in if 1", "[if]")
{
    static const char source[] =
        "#section main_0x100\n"
        "x equ label1\n"
        "y equ label2\n"
        "label1:\n"
        "db 1,2,3\n"
        "label2:\n"
        "#if 0\n"
        "db 0xee\n"
        "#endif\n"
        "#if 1\n"
        "db 0xff\n"
        "#endif\n"
        "dw x,y\n"
        ;

    static const unsigned char binary[] = {
        0x01,
        0x02,
        0x03,
        0xff,
        0x00,
        0x01,
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

// FIXME
TEST_CASE("equ with label in if 2", "[if]")
{
    static const char source[] =
        "#section main\n"
        "x equ label1\n"
        "y equ label2\n"
        "label1:\n"
        "db 1,2,3\n"
        "#if 0\n"
        "db 0xee\n"
        "#endif\n"
        "#if 1\n"
        "db 0xff\n"
        "#endif\n"
        "label2:\n"
        "dw x,y\n"
        ;

    static const unsigned char binary[] = {
        0x01,
        0x02,
        0x03,
        0xff,
        0x34,
        0x12,
        0x38,
        0x12,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("equ with label in if 3", "[if]")
{
    static const char source[] =
        "#section main_0x100\n"
        "x equ label1\n"
        "y equ label2\n"
        "label1:\n"
        "db 1,2,3\n"
        "label2:\n"
        "#if y-x\n"
        "db 0xff\n"
        "#endif\n"
        "#if y-y\n"
        "db 0xcc\n"
        "#endif\n"
        "dw x,y\n"
        ;

    static const unsigned char binary[] = {
        0x01,
        0x02,
        0x03,
        0xff,
        0x00,
        0x01,
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

TEST_CASE("equ with label in if 4", "[if]")
{
    static const char source[] =
        "#section main_0x100\n"
        "x equ label1\n"
        "y equ label2\n"
        "label1:\n"
        "db 1,2,3\n"
        "#if y-x\n"
        "db 0xee\n"
        "#endif\n"
        "label2:\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "value for 'label2' is not available in this context");
}

/* FIXME
TEST_CASE("equ with local label in if 1", "[if]")
{
    static const char source[] =
        "section main [base 0x1234]\n"
        "if 1\n"
        "x equ @@1\n"
        "db 0xaa,0xbb\n"
        "@@1 dw x\n"
        "else\n"
        "db 0xcc\n"
        "x equ @@1\n"
        "@@1 dw x\n"
        "endif\n"
        "db 0xdd\n"
        "dw x\n"
        ;

    static const unsigned char binary[] = {
        0xaa,
        0xbb,
        0x36,
        0x12,
        0xdd,
        0x36,
        0x12,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.lastErrorMessage() == "");
    REQUIRE(errorConsumer.errorCount() == 0);
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("equ with local label in if 2", "[if]")
{
    static const char source[] =
        "section main [base 0x1234]\n"
        "if 0\n"
        "x equ @@1\n"
        "db 0xaa,0xbb\n"
        "@@1 dw x\n"
        "else\n"
        "db 0xcc\n"
        "x equ @@1\n"
        "@@1 dw x\n"
        "endif\n"
        "db 0xdd\n"
        "dw x\n"
        ;

    static const unsigned char binary[] = {
        0xcc,
        0x35,
        0x12,
        0xdd,
        0x35,
        0x12,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.lastErrorMessage() == "");
    REQUIRE(errorConsumer.errorCount() == 0);
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}
*/

TEST_CASE("expression as condition", "[if]")
{
    static const char source[] =
        "#section main_0x100\n"
        "value equ 4+(10/2)-3\n"
        "#if value\n"
        "db 0xaa\n"
        "#endif\n"
        "#if value-6\n"
        "db 0xbb\n"
        "#endif\n"
        "#if value-7\n"
        "db 0xcc\n"
        "#endif\n"
        ;

    static const unsigned char binary[] = {
        0xaa,
        0xcc,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("missing endif", "[if]")
{
    static const char source[] =
        "#section main_0x100\n"
        "#if 1\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: missing 'endif'.");
}

TEST_CASE("local labels in if", "[if]")
{
    static const char source[] =
        "#section main_0xff00\n"
        "label:\n"
        "#if 1\n"
        "@@1 db 0xaa\n"
        "#if 1\n"
        "@@2 db 0xbb\n"
        "dw @@1\n"
        "dw @@2\n"
        "#else\n"
        "@@2 db 0xcc\n"
        "dw @@1\n"
        "dw @@2\n"
        "#endif\n"
        "dw @@1\n"
        "#else\n"
        "@@1 db 0xdd\n"
        "#if 1\n"
        "@@2 db 0xee\n"
        "dw @@1\n"
        "dw @@2\n"
        "#endif\n"
        "dw @@1\n"
        "#endif\n"
        ;

    static const unsigned char binary[] = {
        0xaa, // ff00
        0xbb, // ff01
        0x00, // ff02
        0xff, // ff03
        0x01, // ff04
        0xff, // ff05
        0x00, // ff06
        0xff, // ff07
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("local labels context affinity in if 1", "[if]")
{
    static const char source[] =
        "#section main_0x100\n"
        "label:\n"
        "#if 1\n"
        "@@1: db 0\n"
        "#endif\n"
        "@@2:\n"
        "jp label\n"
        "jp label@@2\n"
        "jp label@@1\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "use of undeclared identifier 'label@@1'");
}

TEST_CASE("local labels context affinity in if 2", "[if]")
{
    static const char source[] =
        "#section main\n"
        "label:\n"
        "@@1:\n"
        "#if 1\n"
        "@@1: db 0,0\n"
        "#endif\n"
        "@@2:\n"
        "jp label\n"
        "jp label@@2\n"
        "jp label@@1\n"
        "jp @@1\n"
        ;

    static const unsigned char binary[] = {
        0x00,
        0x00,
        0xc3,
        0x34,
        0x12,
        0xc3,
        0x36,
        0x12,
        0xc3,
        0x34,
        0x12,
        0xc3,
        0x34,
        0x12,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("local labels context affinity in if 3", "[if]")
{
    static const char source[] =
        "#section main\n"
        "label:\n"
        "jp label\n"
        "jp label@@1\n"
        "jp label@@2\n"
        "jp label@@3\n"
        "jp @@1\n"
        "jp @@2\n"
        "jp @@3\n"
        "@@1:\n"
        "db 0x33\n"
        "#if 1\n"
        "@@1: db 0\n"
        "jp @@1\n"
        "jp @@2\n"
        "jp @@3\n"
        "@@11: db 0\n"
        "jp @@11\n"
        "jp @@2\n"
        "jp @@3\n"
        "#endif\n"
        "@@2:\n"
        "db 0x55\n"
        "#if 1\n"
        "jp @@1\n"
        "jp @@2\n"
        "jp @@3\n"
        "@@1: db 1\n"
        "jp @@11\n"
        "jp @@2\n"
        "jp @@3\n"
        "@@11: db 1\n"
        "#endif\n"
        "@@3:\n"
        "db 0x77\n"
        "jp label\n"
        "jp label@@1\n"
        "jp label@@2\n"
        "jp label@@3\n"
        "jp @@1\n"
        "jp @@2\n"
        "jp @@3\n"
        ;

    static const unsigned char binary[] = {
        0xc3, // 0x1234 // jp label
        0x34, // 0x1235
        0x12, // 0x1236
        0xc3, // 0x1237 // jp label@@1
        0x49, // 0x1238
        0x12, // 0x1239
        0xc3, // 0x123a // jp label@@2
        0x5e, // 0x123b
        0x12, // 0x123c
        0xc3, // 0x123d // jp label@@3
        0x73, // 0x123e
        0x12, // 0x123f
        0xc3, // 0x1240 // jp @@1
        0x49, // 0x1241
        0x12, // 0x1242
        0xc3, // 0x1243 // jp @@2
        0x5e, // 0x1244
        0x12, // 0x1245
        0xc3, // 0x1246 // jp @@3
        0x73, // 0x1247
        0x12, // 0x1248
        0x33, // 0x1249 // @@1: db 0x33
        0x00, // 0x124a // @@1: db 0
        0xc3, // 0x124b // jp @@1
        0x4a, // 0x124c
        0x12, // 0x124d
        0xc3, // 0x124e // jp @@2
        0x5e, // 0x124f
        0x12, // 0x1250
        0xc3, // 0x1251 // jp @@3
        0x73, // 0x1252
        0x12, // 0x1253
        0x00, // 0x1254 // db 0
        0xc3, // 0x1255 // jp @@1
        0x54, // 0x1256
        0x12, // 0x1257
        0xc3, // 0x1258 // jp @@2
        0x5e, // 0x1259
        0x12, // 0x125a
        0xc3, // 0x125b // jp @@3
        0x73, // 0x125c
        0x12, // 0x125d
        0x55, // 0x125e // @@2: db 0x55
        0xc3, // 0x125f // jp @@1
        0x68, // 0x1260
        0x12, // 0x1261
        0xc3, // 0x1262 // jp @@2
        0x5e, // 0x1263
        0x12, // 0x1264
        0xc3, // 0x1265 // jp @@3
        0x73, // 0x1266
        0x12, // 0x1267
        0x01, // 0x1268 // @@1: db 1
        0xc3, // 0x1269 // jp @@1
        0x72, // 0x126a
        0x12, // 0x126b
        0xc3, // 0x126c // jp @@2
        0x5e, // 0x126d
        0x12, // 0x126e
        0xc3, // 0x126f // jp @@3
        0x73, // 0x1270
        0x12, // 0x1271
        0x01, // 0x1272 // @@1: db 1
        0x77, // 0x1273 // @@3: db 0x77
        0xc3, // 0x1274 // jp label
        0x34, // 0x1275
        0x12, // 0x1276
        0xc3, // 0x1274 // jp label@@1
        0x49, // 0x1275
        0x12, // 0x1276
        0xc3, // 0x1277 // jp label@@2
        0x5e, // 0x1278
        0x12, // 0x1279
        0xc3, // 0x127a // jp label@@3
        0x73, // 0x127b
        0x12, // 0x127c
        0xc3, // 0x1274 // jp @@1
        0x49, // 0x1275
        0x12, // 0x1276
        0xc3, // 0x1277 // jp @@2
        0x5e, // 0x1278
        0x12, // 0x1279
        0xc3, // 0x127a // jp @@3
        0x73, // 0x127b
        0x12, // 0x127c
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("disallow global labels in if 1", "[if]")
{
    static const char source[] =
        "#section main_0x100\n"
        "#if 1\n"
        "label:\n"
        "#endif\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: global labels are not allowed at this point.");
}

TEST_CASE("disallow global labels in if 2", "[if]")
{
    static const char source[] =
        "#section main_0x100\n"
        "#if 1\n"
        "label@@1:\n"
        "#endif\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: global labels are not allowed at this point.");
}

TEST_CASE("if/repeat nesting 1", "[if]")
{
    static const char source[] =
        "#section main_0x100\n"
        "#if 1\n"
        "db 0xaa\n"
        "#repeat 2\n"
        "db 0xee\n"
        "#if 0\n"
        "db 0xcc\n"
        "#else\n"
        "db 0xdd\n"
        "#endif\n"
        "db 0xff\n"
        "#endrepeat\n"
        "db 0xbb\n"
        "#else\n"
        "db 0x11\n"
        "#repeat 2\n"
        "db 0x22\n"
        "#if 1\n"
        "db 0x33\n"
        "#endif\n"
        "db 0x55\n"
        "#endrepeat\n"
        "db 0x66\n"
        "#endif\n"
        "db 0x77\n"
        "#if 0\n"
        "db 0xaa\n"
        "#repeat 2\n"
        "db 0xee\n"
        "#if 0\n"
        "db 0xcc\n"
        "#else\n"
        "db 0xdd\n"
        "#endif\n"
        "db 0xff\n"
        "#endrepeat\n"
        "db 0xbb\n"
        "#else\n"
        "db 0x11\n"
        "#repeat 1\n"
        "db 0x22\n"
        "#if 0\n"
        "db 0x33\n"
        "#endif\n"
        "db 0x55\n"
        "#endrepeat\n"
        "db 0x66\n"
        "#endif\n"
        "db 0x88\n"
        ;

    static const unsigned char binary[] = {
        0xaa,
        0xee,
        0xdd,
        0xff,
        0xee,
        0xdd,
        0xff,
        0xbb,
        0x77,
        0x11,
        0x22,
        0x55,
        0x66,
        0x88,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("if/repeat nesting 2", "[if]")
{
    static const char source[] =
        "#section main_0x100\n"
        "#repeat 5, cnt\n"
        "#if cnt == 0\n"
        "db 0xaa\n"
        "#endif\n"
        "#if cnt == 1\n"
        "db 0xbb\n"
        "#endif\n"
        "#if cnt == 2\n"
        "db 0xcc\n"
        "#endif\n"
        "#if cnt == 3\n"
        "db 0xdd\n"
        "#endif\n"
        "#if cnt == 4\n"
        "#repeat 2, cnt2\n"
        "#if cnt2 == 0\n"
        "db 0xee\n"
        "#else\n"
        "db 0xff\n"
        "#endif\n"
        "#endrepeat\n"
        "#endif\n"
        "#endrepeat\n"
        ;

    static const unsigned char binary[] = {
        0xaa,
        0xbb,
        0xcc,
        0xdd,
        0xee,
        0xff,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}
