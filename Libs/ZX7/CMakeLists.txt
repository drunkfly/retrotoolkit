
add(ZX7
    STATIC_LIBRARY
    FOLDER
        "Libs"
    INCLUDE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    SOURCES
        src/compress.c
        src/optimize.c
        src/zx7.h
    )
