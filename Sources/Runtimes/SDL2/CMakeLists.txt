
add(SDL2Runtime
    EXECUTABLE
    LIBS
        SDL2
        SDL2main
        Z80
    OUTPUT_DIR
        "${CMAKE_BINARY_DIR}"
    SOURCES
        Common.h
        Cpu.cpp
        Cpu.h
        Game.cpp
        Game.h
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
