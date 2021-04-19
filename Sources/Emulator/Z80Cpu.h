#ifndef EMULATOR_Z80CPU_H
#define EMULATOR_Z80CPU_H

#include "Emulator/Common.h"

class Z80Cpu : public z80::z80_cpu<Z80Cpu>
{
public:
    explicit Z80Cpu(Emulator* emulator);

    z80::fast_u8 on_read(z80::fast_u16 addr);
    void on_write(z80::fast_u16 addr, z80::fast_u8 value);

    z80::fast_u8 on_input(z80::fast_u16 port);
    void on_output(z80::fast_u16 port, z80::fast_u8 value);

    void on_set_pc(z80::fast_u16 value);

    void on_step();
    void on_tick(unsigned t);
    void on_halt();

    void run();

private:
    Emulator* mEmulator;
    size_t mTicksSinceInt;
    bool mHalted;

    DISABLE_COPY(Z80Cpu);
};

#endif
