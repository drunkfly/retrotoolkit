#include "Tests/Common.h"

TEST_CASE("repeat", "[repeat]")
{
    static const char source[] =
        "#section main_0x100\n"
        "#repeat 5\n"
        "db 0x01\n"
        "#endrepeat\n"
        "#repeat 2\n"
        "db 0x1b\n"
        "#repeat 3\n"
        "db 0x2a\n"
        "#endrepeat\n"
        "#endrepeat\n"
        ;

    static const unsigned char binary[] = {
        0x01,
        0x01,
        0x01,
        0x01,
        0x01,
        0x1b,
        0x2a,
        0x2a,
        0x2a,
        0x1b,
        0x2a,
        0x2a,
        0x2a,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("repeat with counter", "[repeat]")
{
    static const char source[] =
        "#section main_0x100\n"
        "#repeat 3, cnt\n"
        "db cnt\n"
        "#endrepeat\n"
        "#repeat 5, cnt\n"
        "db cnt + 9\n"
        "#endrepeat\n"
        "#repeat 3, cnt1\n"
        "db 0x80|cnt1\n"
        "#repeat 4, cnt2\n"
        "db (cnt1+1)*(cnt2+1)\n"
        "#endrepeat\n"
        "#repeat 1\n"
        "db 0xff\n"
        "#endrepeat\n"
        "#endrepeat\n"
        ;

    static const unsigned char binary[] = {
        0x00,
        0x01,
        0x02,
        0x09,
        0x0a,
        0x0b,
        0x0c,
        0x0d,
        0x80,
        0x01,
        0x02,
        0x03,
        0x04,
        0xff,
        0x81,
        0x02,
        0x04,
        0x06,
        0x08,
        0xff,
        0x82,
        0x03,
        0x06,
        0x09,
        0x0c,
        0xff,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("non-conflicting name in repeat", "[repeat]")
{
    static const char source[] =
        "#section main_0x100\n"
        "db 0x30\n"
        "#repeat 0, cnt\n"
        "db 0x14\n"
        "#endrepeat\n"
        "cnt:\n"
        "db 0x40\n"
        ;

    static const unsigned char binary[] = {
        0x30,
        0x40,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("conflicting name in repeat 1", "[repeat]")
{
    static const char source[] =
        "#section main_0x100\n"
        "label1:\n"
        "#repeat 5, label1\n"
        "#endrepeat\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: duplicate identifier \"label1\".");
}

TEST_CASE("conflicting name in repeat 2", "[repeat]")
{
    static const char source[] =
        "#section main_0x100\n"
        "count equ 4\n"
        "#repeat 5, count\n"
        "#endrepeat\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: duplicate identifier \"count\".");
}

TEST_CASE("conflicting name in repeat 3", "[repeat]")
{
    static const char source[] =
        "#section main_0x100\n"
        "#repeat 5, count\n"
        "count equ 4\n"
        "#endrepeat\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: duplicate identifier \"count\".");
}

TEST_CASE("conflicting name in inner repeat", "[repeat]")
{
    static const char source[] =
        "#section main_0x100\n"
        "#repeat 5, cnt1\n"
        "#repeat 3, cnt1\n"
        "#endrepeat\n"
        "#endrepeat\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: duplicate identifier \"cnt1\".");
}

TEST_CASE("equ in repeat", "[repeat]")
{
    static const char source1[] =
        "#section main_0x100\n"
        "db 0xaa,counter\n"
        "#section main1\n"
        "db 0xbb,counter\n"
        "#section main2\n"
        "db 0xee,counter\n"
        ;

    static const char source2[] =
        "#section main1\n"
        "db 0xcc,counter\n"
        "#repeat 4, cnt\n"
        "counter equ cnt*2+1\n"
        "db counter\n"
        "#endrepeat\n"
        "db 0x88,counter\n"
        ;

    static const char source3[] =
        "#section main1\n"
        "db 0xdd,counter\n"
        "#section main2\n"
        "db 0xff,counter\n"
        ;

    static const unsigned char binary[] = {
        0xaa,
        0x01,
        0xbb,
        0x01,
        0xcc,
        0x01,
        0x01,
        0x03,
        0x05,
        0x07,
        0x88,
        0x01,
        0xdd,
        0x01,
        0xee,
        0x01,
        0xff,
        0x01,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble3(errorConsumer, source1, source2, source3);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("equ with label in repeat 1", "[repeat]")
{
    static const char source[] =
        "#section main_0x100\n"
        "x equ label1\n"
        "y equ label2\n"
        "label1:\n"
        "db 1,2,3\n"
        "label2:\n"
        "#repeat y-x\n"
        "db 0xff\n"
        "#endrepeat\n"
        ;

    static const unsigned char binary[] = {
        0x01,
        0x02,
        0x03,
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

TEST_CASE("equ with label in repeat 2", "[repeat]")
{
    static const char source[] =
        "#section main_0x100\n"
        "x equ label1\n"
        "y equ label2\n"
        "label1:\n"
        "db 1,2,3\n"
        "#repeat y-x\n"
        "db 0xff\n"
        "#endrepeat\n"
        "label2:\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: unable to resolve address for label \"label2\".");
}

TEST_CASE("equ with label in repeat 3", "[repeat]")
{
    static const char source[] =
        "#section main\n"
        "x equ label1\n"
        "y equ label2\n"
        "label1:\n"
        "db 1,2,3\n"
        "#repeat 2\n"
        "dw y\n"
        "#endrepeat\n"
        "label2:\n"
        ;

    static const unsigned char binary[] = {
        0x01,
        0x02,
        0x03,
        0x3b,
        0x12,
        0x3b,
        0x12,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("expression as repeat counter", "[repeat]")
{
    static const char source[] =
        "#section main_0x100\n"
        "count equ 4+(10/2)-3\n"
        "#repeat count, cnt\n"
        "db cnt+0x40\n"
        "#repeat cnt, cnt2\n"
        "db cnt2|0x80\n"
        "#endrepeat\n"
        "#endrepeat\n"
        ;

    static const unsigned char binary[] = {
        0x40,
        0x41,
        0x80,
        0x42,
        0x80,
        0x81,
        0x43,
        0x80,
        0x81,
        0x82,
        0x44,
        0x80,
        0x81,
        0x82,
        0x83,
        0x45,
        0x80,
        0x81,
        0x82,
        0x83,
        0x84,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("missing endrepeat", "[repeat]")
{
    static const char source[] =
        "#section main_0x100\n"
        "#repeat 5\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: missing 'endrepeat'.");
}

TEST_CASE("negative repeat count", "[repeat]")
{
    static const char source[] =
        "#section main_0x100\n"
        "#repeat -1\n"
        "#endrepeat\n"
        "db 0\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: repeat counter is negative.");
}

TEST_CASE("too large repeat count", "[repeat]")
{
    static const char source[] =
        "#section main_0x100\n"
        "#repeat 0x10000\n"
        "#endrepeat\n"
        "db 0\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: repeat counter is too large.");
}

TEST_CASE("local labels in repeat", "[repeat]")
{
    static const char source[] =
        "#section main_0xff00\n"
        "label:\n"
        "#repeat 2, cnt1\n"
        "@@1 db 0x08|cnt1\n"
        "@@2 db 0x04|cnt1\n"
        "dw @@1\n"
        "dw @@2\n"
        "#repeat 2, cnt2\n"
        "@@3 db ((0x08|cnt1)<<4)|(cnt2|0x08)\n"
        "    db ((0x04|cnt1)<<4)|(cnt2|0x04)\n"
        "dw @@3\n"
        "dw @@2\n"
        "#endrepeat\n"
        "dw @@1\n"
        "dw @@2\n"
        "#endrepeat\n"
        ;

    static const unsigned char binary[] = {
        0x08, // ff00
        0x04, // ff01
        0x00, // ff02
        0xff, // ff03
        0x01, // ff04
        0xff, // ff05
        0x88, // ff06
        0x44, // ff07
        0x06, // ff08
        0xff, // ff09
        0x01, // ff0a
        0xff, // ff0b
        0x89, // ff0c
        0x45, // ff0d
        0x0c, // ff0e
        0xff, // ff0f
        0x01, // ff10
        0xff, // ff11
        0x00, // ff12
        0xff, // ff13
        0x01, // ff14
        0xff, // ff15
        0x09, // ff16
        0x05, // ff17
        0x16, // ff18
        0xff, // ff19
        0x17, // ff1a
        0xff, // ff1b
        0x98, // ff1c
        0x54, // ff1d
        0x1c, // ff1e
        0xff, // ff1f
        0x17, // ff20
        0xff, // ff21
        0x99, // ff22
        0x55, // ff23
        0x22, // ff24
        0xff, // ff25
        0x17, // ff26
        0xff, // ff27
        0x16, // ff28
        0xff, // ff29
        0x17, // ff2a
        0xff, // ff2b
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("local labels in repeat 2", "[repeat]")
{
    static const char source[] =
        "#section main_0xff00\n"
        "label:\n"
        "#repeat 2, cnt1\n"
        "@@1 db 0x08|cnt1\n"
        "@@2 db 0x04|cnt1\n"
        "dw @@1\n"
        "dw @@2\n"
        "#repeat 2, cnt2\n"
        "dw @@3\n"
        "@@3 db ((0x08|cnt1)<<4)|(cnt2|0x08)\n"
        "    db ((0x04|cnt1)<<4)|(cnt2|0x04)\n"
        "dw @@2\n"
        "#endrepeat\n"
        "dw @@1\n"
        "dw @@2\n"
        "#endrepeat\n"
        ;

    static const unsigned char binary[] = {
              // -------------------------------
        0x08, // ff00 @@1 cnt1==0
        0x04, // ff01 @@2 cnt1==0
        0x00, // ff02 dw @@1
        0xff, // ff03
        0x01, // ff04 dw @@2
        0xff, // ff05
        0x08, // ff06 dw @@3
        0xff, // ff07
        0x88, // ff08 @@3 cnt1==0 cnt2==0
        0x44, // ff09
        0x01, // ff0a dw @@2
        0xff, // ff0b
        0x0e, // ff0c dw @@3
        0xff, // ff0d
        0x89, // ff0e @@3 cnt1==0 cnt2==1
        0x45, // ff0f
        0x01, // ff10 dw @@2
        0xff, // ff11
        0x00, // ff12 dw @@1
        0xff, // ff13
        0x01, // ff14 dw @@2
        0xff, // ff15
              // -------------------------------
        0x09, // ff16 @@1 cnt1==0
        0x05, // ff17 @@2 cnt1==0
        0x16, // ff18 dw @@1
        0xff, // ff19
        0x17, // ff1a dw @@2
        0xff, // ff1b
        0x1e, // ff1c dw @@3
        0xff, // ff1d
        0x98, // ff1e @@3 cnt1==0 cnt2==0
        0x54, // ff1f
        0x17, // ff20 dw @@2
        0xff, // ff21
        0x24, // ff22 dw @@3
        0xff, // ff23
        0x99, // ff24 @@3 cnt1==0 cnt2==1
        0x55, // ff25
        0x17, // ff26 dw @@2
        0xff, // ff27
        0x16, // ff28 dw @@1
        0xff, // ff29
        0x17, // ff2a dw @@2
        0xff, // ff2b
    };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("local labels in repeat 3", "[repeat]")
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
        "#repeat 2\n"
        "@@4: db 0\n"
        "jp @@4\n"
        "jp @@2\n"
        "jp @@3\n"
        "#endrepeat\n"
        "@@2:\n"
        "db 0x55\n"
        "#repeat 2\n"
        "jp @@5\n"
        "jp @@2\n"
        "jp @@3\n"
        "@@5: db 1\n"
        "#endrepeat\n"
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

TEST_CASE("local labels in repeat 4", "[repeat]")
{
    static const char source[] =
        /* 1*/ "#section main_0xff00\n"
        /* 2*/ "label:\n"
        /* 3*/ "#repeat 2, cnt1\n"
        /* 4*/ "@@1 db 0x08|cnt1\n"
        /* 5*/ "@@2 db 0x04|cnt1\n"
        /* 6*/ "dw @@1\n"
        /* 7*/ "dw @@2\n"
        /* 8*/ "#repeat 2, cnt2\n"
        /* 9*/ "#if cnt2 == 0\n"
        /*10*/ "@@3 db ((0x08|cnt1)<<4)|(cnt2|0x08)\n"
        /*11*/ "    db ((0x04|cnt1)<<4)|(cnt2|0x04)\n"
        /*12*/ "#endif\n"
        /*13*/ "dw @@3\n"
        /*14*/ "#if cnt2 == 1\n"
        /*15*/ "@@3 db ((0x08|cnt1)<<4)|(cnt2|0x08)\n"
        /*16*/ "    db ((0x04|cnt1)<<4)|(cnt2|0x04)\n"
        /*17*/ "#endif\n"
        /*18*/ "dw @@2\n"
        /*19*/ "#endrepeat\n"
        /*20*/ "dw @@1\n"
        /*21*/ "dw @@3\n"
        /*22*/ "#endrepeat\n"
        ;

    static const unsigned char binary[] = {
              // -------------------------------
        0x08, // ff00 @@1 cnt1==0
        0x04, // ff01 @@2 cnt1==0
        0x00, // ff02 dw @@1
        0xff, // ff03
        0x01, // ff04 dw @@2
        0xff, // ff05
        0x88, // ff06 @@3 cnt1==0 cnt2==0
        0x44, // ff07
        0x06, // ff08 dw @@3
        0xff, // ff09
        0x01, // ff0a dw @@2
        0xff, // ff0b
        0x0e, // ff0c dw @@3
        0xff, // ff0d
        0x89, // ff0e @@3 cnt1==0 cnt2==1
        0x45, // ff0f
        0x01, // ff10 dw @@2
        0xff, // ff11
        0x00, // ff12 dw @@1
        0xff, // ff13
        0x06, // ff14 dw @@3
        0xff, // ff15
              // -------------------------------
        0x09, // ff16 @@1 cnt1==1
        0x05, // ff17 @@2 cnt1==1
        0x16, // ff18 dw @@1
        0xff, // ff19
        0x17, // ff1a dw @@2
        0xff, // ff1b
        0x98, // ff1c @@3 cnt1==1 cnt2==0
        0x54, // ff1d
        0x1c, // ff1e dw @@3
        0xff, // ff1f
        0x17, // ff20 dw @@2
        0xff, // ff21
        0x24, // ff22 dw @@3
        0xff, // ff23
        0x99, // ff24 @@3 cnt1==1 cnt2==1
        0x55, // ff25
        0x17, // ff26 dw @@2
        0xff, // ff27
        0x16, // ff28 dw @@1
        0xff, // ff29
        0x1c, // ff2a dw @@3
        0xff, // ff2b
    };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("global labels in repeat 1", "[repeat]")
{
    static const char source[] =
        "#section main_0x100\n"
        "#repeat 2\n"
        "label:\n"
        "dw label\n"
        "#endrepeat\n"
        "dw label\n"
        ;

    static const unsigned char binary[] = {
        0x00,
        0x01,
        0x02,
        0x01,
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

TEST_CASE("global labels in repeat 2", "[repeat]")
{
    static const char source[] =
        "#section main_0x100\n"
        "#repeat 2\n"
        "label@@1:\n"
        "db 1\n"
        "#endrepeat\n"
        "dw label@@1\n"
        ;

    static const unsigned char binary[] = {
        0x01,
        0x01,
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

TEST_CASE("relative local labels in repeat", "[repeat]")
{
    static const char source[] =
        "#section main_0x100\n"
        "label1:\n"
        "#repeat 2\n"
        "@@local:\n"
        "djnz @@local\n"
        "#endrepeat\n"
        "label2:\n"
        "#repeat 2\n"
        "@@local:\n"
        "jr @@local\n"
        "#endrepeat\n"
        "label3:\n"
        "#repeat 2\n"
        "djnz @@local\n"
        "@@local:\n"
        "#endrepeat\n"
        "label4:\n"
        "#repeat 2\n"
        "jr @@local\n"
        "@@local:\n"
        "#endrepeat\n"
        ;

    static const unsigned char binary[] = {
        0x10,
        0xfe,
        0x10,
        0xfe,
        0x18,
        0xfe,
        0x18,
        0xfe,
        0x10,
        0x00,
        0x10,
        0x00,
        0x18,
        0x00,
        0x18,
        0x00,
        };

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    DataBlob expected(binary, sizeof(binary));
    REQUIRE(errorConsumer.errorMessage() == "");
    REQUIRE(actual == expected);
    REQUIRE(!actual.hasFiles());
}

TEST_CASE("conflicting local labels in repeat", "[repeat]")
{
    static const char source[] =
        "#section main_0x100\n"
        "label:\n"
        "#repeat 2\n"
        "@@local:\n"
        "djnz @@local\n"
        "#endrepeat\n"
        "#repeat 2\n"
        "@@local:\n"
        "jr @@local\n"
        "#endrepeat\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:8: duplicate identifier \"label@@local\".");
}
