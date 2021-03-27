cmake_minimum_required(VERSION 3.16)
project(RetroToolkit)

include(CMake/Common.cmake)
include(CMake/Bin2C.cmake)

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT "RetroToolkit")

add_subdirectory(Libs)
add_subdirectory(Sources)
