
add(Tests
    EXECUTABLE
    CONSOLE
    FOLDER
        "Tests"
    OUTPUT_DIR
        "${CMAKE_CURRENT_BINARY_DIR}"
    LIBS
        Common
        Compiler
        Catch
    SOURCES
        Util/DataBlob.cpp
        Util/DataBlob.h
        Util/ErrorConsumer.cpp
        Util/ErrorConsumer.h
        Util/TestUtil.cpp
        Util/TestUtil.h
        CaseTests.cpp
        Common.h
        DataTests.cpp
        EquTests.cpp
        ErrorTests.cpp
        ExprTests.cpp
        IfTests.cpp
        LabelTests.cpp
        OpcodeTests.cpp
        RepeatTests.cpp
        main.cpp
    )

target_precompile_headers(Tests
    PRIVATE
        [["Tests/Common.h"]]
    )

set_source_files_properties(main.cpp PROPERTIES
    SKIP_PRECOMPILE_HEADERS TRUE
    )

set(result_cpp "${CMAKE_CURRENT_BINARY_DIR}/testresult.cpp")
source_group("Generated Files" FILES "${result_cpp}")

add(RunTests
    STATIC_LIBRARY
    FOLDER
        "Tests"
    SOURCES
        "${result_cpp}"
    )

set(use_colour "")
if(WIN32)
    set(use_colour "--use-colour no")
endif()

add_custom_command(OUTPUT "${result_cpp}"
    COMMAND Tests ${use_colour} --reporter compact --resultfile "${result_cpp}"
    DEPENDS Tests
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Running tests"
    )
