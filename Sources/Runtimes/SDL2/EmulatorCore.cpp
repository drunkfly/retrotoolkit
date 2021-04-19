// SDL2 runtime uses different MSVC runtime so we have to recompile emulator code separately. Use Unity build here.

#include "Emulator/Emulator.cpp"
#include "Emulator/Z80Cpu.cpp"
#include "Emulator/Z80Memory.cpp"
#include "Emulator/Z80Screen.cpp"

#ifndef NDEBUG
#include "Emulator/Util/LargeBuffer.cpp"
#include "Emulator/Snapshot.cpp"
#include "Emulator/Disassembler.cpp"
#endif
