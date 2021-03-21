
if(WIN32)
    add_definitions(-DWIN32)
endif()

add(FuseUtils
    STATIC_LIBRARY
    FOLDER
        "Libs"
    LIBS
        LibSpectrum
    SOURCES
        compat.h
        tape2wav.c
    )
