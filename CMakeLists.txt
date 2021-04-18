cmake_minimum_required(VERSION 3.16)
project(RetroToolkit)

include(CMake/Common.cmake)
include(CMake/Bin2C.cmake)

if(NOT EXCLUDE_GUI)
    set_directory_properties(PROPERTIES VS_STARTUP_PROJECT "RetroToolkit")
else()
    set_directory_properties(PROPERTIES VS_STARTUP_PROJECT "SDL2Runtime")
endif()

add_subdirectory(Libs)
add_subdirectory(Sources)
