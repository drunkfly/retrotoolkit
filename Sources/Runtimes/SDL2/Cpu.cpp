#include "Cpu.h"
#include "Runtimes/SDL2/Screen.h"
#include "Runtimes/SDL2/Memory.h"

namespace
{
    #pragma pack(push, 1)

    struct RegistersInit
    {
        uint8_t a;
        uint8_t f;
        uint8_t b;
        uint8_t c;
        uint8_t d;
        uint8_t e;
        uint8_t l;
        uint8_t h;
        uint8_t shadowA;
        uint8_t shadowF;
        uint8_t shadowB;
        uint8_t shadowC;
        uint8_t shadowD;
        uint8_t shadowE;
        uint8_t shadowL;
        uint8_t shadowH;
        uint8_t ixH;
        uint8_t ixL;
        uint8_t iyH;
        uint8_t iyL;
        uint8_t i;
        uint8_t r;
        uint8_t intMode;
        uint8_t intDisabled;
        uint8_t pcL;
        uint8_t pcH;
        uint8_t spL;
        uint8_t spH;
        uint8_t port1FFD;
        uint8_t port7FFD;
        uint8_t portFFFD;
    };

    #pragma pack(pop)

    RegistersInit regsInit = { 'R', 'E', 'G', 'I', 'S', 'T', 'E', 'R', 'S', '\x1A', '\x1A' };
}

Cpu::Cpu(Screen* screen)
    : mScreen(screen)
    , mCurrentBank(0)
    , mHalted(false)
{
    set_a(regsInit.a);
    set_f(regsInit.f);
    set_b(regsInit.b);
    set_c(regsInit.c);
    set_d(regsInit.d);
    set_e(regsInit.e);
    set_h(regsInit.h);
    set_l(regsInit.l);
    set_alt_af((regsInit.shadowA << 8) | regsInit.shadowF);
    set_alt_bc((regsInit.shadowB << 8) | regsInit.shadowC);
    set_alt_de((regsInit.shadowD << 8) | regsInit.shadowE);
    set_alt_hl((regsInit.shadowH << 8) | regsInit.shadowL);
    set_ix((regsInit.ixH << 8) | regsInit.ixL);
    set_iy((regsInit.iyH << 8) | regsInit.iyL);
    set_i(regsInit.i);
    set_r(regsInit.r);
    set_int_mode(regsInit.intMode);
    set_is_int_disabled(regsInit.intDisabled != 0);
    set_pc((regsInit.pcH << 8) | regsInit.pcL);
    set_sp((regsInit.spH << 8) | regsInit.spL);
    on_output(0x1FFD, regsInit.port1FFD);
    on_output(0x7FFD, regsInit.port7FFD);
    on_output(0xFFFD, regsInit.portFFFD);
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
