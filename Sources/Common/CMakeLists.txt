
add(Common
    STATIC_LIBRARY
    LIBS
        TinyXML
    SOURCES
        Common.h
        GC.cpp
        GC.h
        IO.cpp
        IO.h
        StreamUtils.cpp
        StreamUtils.h
        Strings.cpp
        Strings.h
        StringSet.h
        TemplateMagic.h
        Xml.cpp
        Xml.h
    )

target_precompile_headers(Common
    PRIVATE
        [["Common/Common.h"]]
    )
