#include <catch.hpp>
#include "Tests/Util/TestUtil.h"

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
