#include <catch.hpp>
#include "Tests/Util/TestUtil.h"

TEST_CASE("expected opcode", "[errors]")
{
    static const char source[] =
        "#section main_0x100\n"
        "4\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: expected opcode or directive.");
}

TEST_CASE("invalid opcode", "[errors]")
{
    static const char source[] =
        "#section main_0x100\n"
        "x\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:2: unknown opcode \"x\".");
}

TEST_CASE("invalid label 1", "[errors]")
{
    static const char source[] =
        "#section main_0x100\n"
        "y:\n"
        "@@x\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: missing ':' after local label name.");
}

TEST_CASE("invalid label 2", "[errors]")
{
    static const char source[] =
        "#section main_0x100\n"
        "y:\n"
        "@@x"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: missing ':' after local label name.");
}

TEST_CASE("invalid label 3", "[errors]")
{
    static const char source[] =
        "#section main_0x100\n"
        "y:\n"
        "@@x ld a,4\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: missing ':' after local label name.");
}

TEST_CASE("invalid label 4", "[errors]")
{
    static const char source[] =
        "#section main_0x100\n"
        "y:\n"
        "@@x\n"
        "db 8\n"
        ;

    ErrorConsumer errorConsumer;
    DataBlob actual = assemble(errorConsumer, source);
    REQUIRE(errorConsumer.errorMessage() == "source:3: unexpected end of line.");
}
