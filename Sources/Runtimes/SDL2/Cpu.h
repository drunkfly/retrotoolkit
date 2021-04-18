#ifndef RUNTIMES_SDL2_CPU_H
#define RUNTIMES_SDL2_CPU_H

#include <z80.h>

class Cpu : public z80::z80_cpu<Cpu>
{
};

#endif
