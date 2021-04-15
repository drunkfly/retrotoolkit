#ifndef RUNTIMES_SDL2_COMMON_H
#define RUNTIMES_SDL2_COMMON_H

#include <SDL.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <memory>
#include <stdexcept>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

class SDL2Core;
class Settings;
class QuitGame {};

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 192

#define BORDER_SIZE 32

#define DISABLE_COPY(NAME) \
    NAME(const NAME&) = delete; \
    NAME& operator=(const NAME&) = delete

#endif
