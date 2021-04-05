
add(ZX0
    STATIC_LIBRARY
    FOLDER
        "Libs"
    INCLUDE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    SOURCES
        src/compress.c
        src/memory.c
        src/optimize.c
        src/zx0.h
    )
