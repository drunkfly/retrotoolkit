
add(TinyXML
    STATIC_LIBRARY
    FOLDER
        "Libs"
    INCLUDE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}"
    DEFINES
        TIXML_USE_STL
    SOURCES
        tinyxml.cpp
        tinyxml.h
        tinyxmlerror.cpp
        tinyxmlparser.cpp
    )
