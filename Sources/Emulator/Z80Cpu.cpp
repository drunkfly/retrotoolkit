#include "Z80Cpu.h"
#include "Emulator/Emulator.h"
#include "Emulator/Z80Screen.h"
#include "Emulator/Z80Memory.h"

static const size_t TicksPerFrame = 69888;

Z80Cpu::Z80Cpu(Emulator* emulator)
    : mEmulator(emulator)
    , mTicksSinceInt(0)
    , mHalted(false)
{
}

z80::fast_u8 Z80Cpu::on_read(z80::fast_u16 addr)
{
    return mEmulator->memory()->read(addr);
}

void Z80Cpu::on_write(z80::fast_u16 addr, z80::fast_u8 value)
{
    mEmulator->memory()->write(addr, value);
}

z80::fast_u8 Z80Cpu::on_input(z80::fast_u16 port)
{
    return 0;
}

void Z80Cpu::on_output(z80::fast_u16 port, z80::fast_u8 value)
{
    if ((port & 0xff) == 0xfe)
        mEmulator->screen()->setBorderColor(value & 7);
    else if (port == 0x7ffd) {
        mEmulator->memory()->setCurrentBank(value & 7);
        mEmulator->screen()->setUseShadowScreen((value & 8) != 0);
    }
}

void Z80Cpu::on_set_pc(z80::fast_u16 value)
{
    if (value < 0x4000) {
        assert(false);
        throw std::runtime_error("attempted to access ROM memory.");
    }

    z80_cpu::on_set_pc(value);
}

void Z80Cpu::on_step()
{
    z80_cpu::on_step();
}

void Z80Cpu::on_tick(unsigned t)
{
    mTicksSinceInt += t;
}

void Z80Cpu::on_halt()
{
    z80_cpu::on_halt();
    mHalted = true;
}

void Z80Cpu::run()
{
    while (!mHalted && mTicksSinceInt < TicksPerFrame)
        on_step();

    mTicksSinceInt = 0;
    mHalted = false;
    on_handle_active_int();
    mEmulator->screen()->onScreenInterrupt();
}
