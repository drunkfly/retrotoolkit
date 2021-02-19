cmake_minimum_required(VERSION 3.16)
project(ZXToolkit)

include(CMake/Common.cmake)
set_directory_properties(PROPERTIES VS_STARTUP_PROJECT "ZXToolkit")

add_subdirectory(Sources)
