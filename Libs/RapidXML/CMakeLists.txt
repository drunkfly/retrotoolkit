
add(RapidXML
    HEADER_ONLY_LIBRARY
    FOLDER
        "Libs"
    INCLUDE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}"
    SOURCES
        rapidxml.hpp
        rapidxml_iterators.hpp
        rapidxml_print.hpp
        rapidxml_utils.hpp
    )
