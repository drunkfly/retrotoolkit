
add(SDL2Runtime
    EXECUTABLE
    LIBS
        SDL2
    SOURCES
        Common.h
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
