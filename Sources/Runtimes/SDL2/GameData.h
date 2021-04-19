#ifndef RUNTIMES_SDL2_MEMORY_H
#define RUNTIMES_SDL2_MEMORY_H

#include "Runtimes/SDL2/Common.h"
#include "Emulator/Z80Memory.h"

extern SnapshotState initState;
extern uint8_t initMemory[Z80Memory::BankCount * Z80Memory::BankSize];

#endif
