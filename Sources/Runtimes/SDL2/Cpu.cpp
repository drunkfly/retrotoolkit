#include "Cpu.h"
#include "Runtimes/SDL2/Screen.h"
#include "Runtimes/SDL2/Memory.h"

Cpu::Cpu(Screen* screen)
    : mScreen(screen)
    , mCurrentBank(0)
    , mHalted(false)
{
}

z80::fast_u8 Cpu::on_read(z80::fast_u16 addr)
{
    if (addr < 0x4000)
        abort();
    else if (addr < 0x8000)
        return memory[addr - 0x4000 + 16384 * 5];
    else if (addr < 0xc000)
        return memory[addr - 0x8000 + 16384 * 5];
    else
        return memory[addr - 0xc000 + 16384 * mCurrentBank];
}

void Cpu::on_write(z80::fast_u16 addr, z80::fast_u8 value)
{
    if (addr < 0x4000)
        abort();
    else if (addr < 0x8000)
        memory[addr - 0x4000 + 16384 * 5] = uint8_t(value);
    else if (addr < 0xc000)
        memory[addr - 0x8000 + 16384 * 5] = uint8_t(value);
    else
        memory[addr - 0xc000 + 16384 * mCurrentBank] = uint8_t(value);
}

z80::fast_u8 Cpu::on_input(z80::fast_u16 port)
{
    return 0;
}

void Cpu::on_output(z80::fast_u16 port, z80::fast_u8 value)
{
    if (port == 0x7ffd) {
        mCurrentBank = value & 7;
        mScreen->setUseShadowScreen((value & 8) != 0);
    }
}

void Cpu::on_halt()
{
    z80_cpu::on_halt();
    mHalted = true;
}

void Cpu::runFrame()
{
    while (!mHalted)
        on_step();

    mHalted = false;
    on_handle_active_int();
}
