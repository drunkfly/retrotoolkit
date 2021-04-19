#ifndef EMULATOR_COMMON_H
#define EMULATOR_COMMON_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <z80.h>

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 192

#define BORDER_SIZE 32

#ifndef DISABLE_COPY
#define DISABLE_COPY(NAME) \
    NAME(const NAME&) = delete; \
    NAME& operator=(const NAME&) = delete
#endif

class Emulator;
class Z80Cpu;
class Z80Memory;
class Z80Screen;
struct SnapshotState;

#endif
