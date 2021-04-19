
add(SDL2Runtime
    EXECUTABLE
    FOLDER
        "Runtimes"
    OUTPUT_DIR
        "${CMAKE_BINARY_DIR}"
    LIBS
        SDL2
        SDL2main
        Z80
    SOURCES
        Common.h
        EmulatorCore.cpp
        Game.cpp
        Game.h
        GameData.cpp
        GameData.h
        SDL2Core.cpp
        SDL2Core.h
        Settings.cpp
        Settings.h
        main.cpp
    )

target_precompile_headers(SDL2Runtime
    PRIVATE
        [["Runtimes/SDL2/Common.h"]]
    )

set_target_properties(SDL2Runtime PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
