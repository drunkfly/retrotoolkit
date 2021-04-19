#ifndef RUNTIMES_SDL2_COMMON_H
#define RUNTIMES_SDL2_COMMON_H

#include "Emulator/Common.h"
#include <SDL.h>

#ifdef __EMSCRIPTEN__
 #include <emscripten.h>
#endif

#ifdef _WIN32
 #define WIN32_LEAN_AND_MEAN
 #define NOMINMAX
 #include <windows.h>
#endif

class SDL2Core;
class Settings;
class QuitGame {};

#endif
