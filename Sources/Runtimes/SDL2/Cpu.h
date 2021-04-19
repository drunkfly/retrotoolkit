#ifndef RUNTIMES_SDL2_CPU_H
#define RUNTIMES_SDL2_CPU_H

#include "Runtimes/SDL2/Common.h"

class Screen;

class Cpu : public z80::z80_cpu<Cpu>
{
public:
    explicit Cpu(Screen* screen);

    z80::fast_u8 on_read(z80::fast_u16 addr);
    void on_write(z80::fast_u16 addr, z80::fast_u8 value);

    z80::fast_u8 on_input(z80::fast_u16 port);
    void on_output(z80::fast_u16 port, z80::fast_u8 value);

    void on_halt();

    void runFrame();

private:
    Screen* mScreen;
    int mCurrentBank;
    bool mHalted;

    DISABLE_COPY(Cpu);
};

#endif
