#include "Cpu.h"
#include "Runtimes/SDL2/Disassembler.h"
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
        uint8_t borderColor;
    };

    #pragma pack(pop)

    RegistersInit regsInit = { 'R', 'E', 'G', 'I', 'S', 'T', 'E', 'R', 'S', '\x1A', '\x1A' };
}

static const size_t TicksPerFrame = 69888;

Cpu::Cpu(Screen* screen)
    : mScreen(screen)
    , mTicksSinceInt(0)
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
    screen->setBorderColor(regsInit.borderColor);
}

size_t Cpu::resolveAddress(z80::fast_u16 addr) const
{
    if (addr < 0x4000)
        abort();
    else if (addr < 0x8000)
        return addr - 0x4000 + 16384 * 5;
    else if (addr < 0xc000)
        return addr - 0x8000 + 16384 * 2;
    else
        return addr - 0xc000 + 16384 * mCurrentBank;
}

z80::fast_u8 Cpu::on_read(z80::fast_u16 addr)
{
    return memory[resolveAddress(addr)];
}

void Cpu::on_write(z80::fast_u16 addr, z80::fast_u8 value)
{
    memory[resolveAddress(addr)] = value;
}

z80::fast_u8 Cpu::on_input(z80::fast_u16 port)
{
    return 0;
}

void Cpu::on_output(z80::fast_u16 port, z80::fast_u8 value)
{
    if ((port & 0xff) == 0xfe)
        mScreen->setBorderColor(value & 7);
    else if (port == 0x7ffd) {
        mCurrentBank = value & 7;
        mScreen->setUseShadowScreen((value & 8) != 0);
    }
}

void Cpu::on_set_pc(z80::fast_u16 value)
{
    if (value < 0x4000)
        abort();
    z80_cpu::on_set_pc(value);
}

void Cpu::on_step()
{
    /*
  #ifndef NDEBUG
    Disassembler disasm(this);
    disasm.setPrependAddress(true);
    disasm.setAppendNewline(true);
   #ifdef _WIN32
    OutputDebugStringA(disasm.on_disassemble());
   #else
    fputs(disasm.on_disassemble(), stdout);
   #endif
  #endif
    */

    z80_cpu::on_step();
}

void Cpu::on_tick(unsigned t)
{
    mTicksSinceInt += t;
}

void Cpu::on_halt()
{
    z80_cpu::on_halt();
    mHalted = true;
}

void Cpu::runFrame()
{
    while (!mHalted && mTicksSinceInt < TicksPerFrame)
        on_step();

    mTicksSinceInt = 0;
    mHalted = false;
    on_handle_active_int();
    mScreen->onScreenInterrupt();
}

#ifndef NDEBUG

void Cpu::loadZ80(const char* path)
{
    // Note: this code is insecure and mostly does not validate input data.
    // It is intended only for debugging and should be excluded from Release builds.

    memset(memory, 0, sizeof(memory));

    static uint8_t buffer[1048576];
    FILE* f = fopen(path, "rb");
    if (!f)
        throw std::runtime_error("Can't open file.");
    size_t bytesRead = fread(buffer, 1, sizeof(buffer), f);
    if (ferror(f))
        throw std::runtime_error("Can't read file.");
    fclose(f);

    // Because of compatibility, if byte 12 is 255, it has to be regarded as being 1.
    if (buffer[12] == 0xff)
        buffer[12] = 1;

    regsInit.a = buffer[0];
    regsInit.f = buffer[1];
    regsInit.c = buffer[2];
    regsInit.b = buffer[3];
    regsInit.l = buffer[4];
    regsInit.h = buffer[5];
    regsInit.pcL = buffer[6];
    regsInit.pcH = buffer[7];
    regsInit.spL = buffer[8];
    regsInit.spH = buffer[9];
    regsInit.i = buffer[10];
    regsInit.r = (buffer[11] & 0x7f) | ((buffer[12] & 1) << 7);
    regsInit.borderColor = (buffer[12] >> 1) & 7;
    bool compressed = (buffer[12] & 0x20) != 0;
    regsInit.e = buffer[13];
    regsInit.d = buffer[14];
    regsInit.shadowC = buffer[15];
    regsInit.shadowB = buffer[16];
    regsInit.shadowE = buffer[17];
    regsInit.shadowD = buffer[18];
    regsInit.shadowL = buffer[19];
    regsInit.shadowH = buffer[20];
    regsInit.shadowA = buffer[21];
    regsInit.shadowF = buffer[22];
    regsInit.iyL = buffer[23];
    regsInit.iyH = buffer[24];
    regsInit.ixL = buffer[25];
    regsInit.ixH = buffer[26];
    regsInit.intDisabled = (buffer[27] == 0);
    regsInit.intMode = buffer[29] & 3;

    if (regsInit.pcL != 0 || regsInit.pcH != 0) {
        // version 1
        if (!compressed) {
            memcpy(&memory[5 * 16384], &buffer[30 + 0 * 16384], 16384);
            memcpy(&memory[2 * 16384], &buffer[30 + 1 * 16384], 16384);
            memcpy(&memory[0 * 16384], &buffer[30 + 2 * 16384], 16384);
        } else {
            size_t off = 30;
            uint8_t* dst = &memory[5 * 16384];
            for (;;) {
                if (buffer[off] == 0 && buffer[off + 1] == 0xED && buffer[off + 2] == 0xED && buffer[off + 3] == 0)
                    break;
                else if (buffer[off] != 0xED || buffer[off + 1] != 0xED)
                    *dst++ = buffer[off++];
                else {
                    off += 2;
                    uint8_t count = buffer[off++];
                    uint8_t byte = buffer[off++];
                    memset(dst, byte, count);
                    dst += count;
                }
            }

            memcpy(&memory[2 * 16384], &memory[6 * 16384], 16384);
            memcpy(&memory[0 * 16384], &memory[7 * 16384], 16384);
            memset(&memory[6 * 16384], 0, 16384);
            memset(&memory[7 * 16384], 0, 16384);
        }
    } else {
        // version 2 or 3
        uint8_t v;
        uint16_t length = buffer[30] | (buffer[31] << 8);
        if (length == 23)
            v = 2;
        else if (length == 54 || length == 55)
            v = 3;
        else
            throw std::runtime_error("Unsupported z80 file version.");

        regsInit.pcL = buffer[32];
        regsInit.pcH = buffer[33];

        uint8_t machine = buffer[34];
        bool is48k = false, isTimex = false;
        switch (machine) {
            case 0:
            case 1:
                is48k = true;
                break;
            case 3:
                if (v == 3)
                    is48k = true;
                break;
            case 14:
            case 15:
            case 128:
                is48k = true;
                isTimex = true;
                break;
            default:
                throw std::runtime_error("Unsupported z80 hardware mode.");
        }

        regsInit.port7FFD = 0;
        if (isTimex)
            ; // FIXME: [35] last OUT to 0xf4
        else if (!is48k)
            regsInit.port7FFD = buffer[35];

        if (isTimex)
            ; // FIXME: [36] last OUT to 0xff

        regsInit.portFFFD = buffer[38];
        regsInit.port1FFD = (length == 55 ? buffer[86] : 0);

        size_t off = 32 + length;
        while (off < bytesRead) {
            length = buffer[off] | (buffer[off + 1] << 8);
            uint8_t bank = buffer[off + 2];
            off += 3;

            if (!is48k) {
                if (bank < 3 || bank > 10) {
                    off += length;
                    continue;
                }
                bank -= 3;
            } else {
                switch (bank) {
                    case 4: bank = 2; break;
                    case 5: bank = 0; break;
                    case 8: bank = 5; break;
                    default: off += length; continue;
                }
            }

            uint8_t* dst = &memory[bank * 16384];
            if (length == 0xffff) {
                memcpy(dst, &buffer[off], 16384);
                off += 16384;
            } else {
                while (length > 0) {
                    if (length < 4 || buffer[off] != 0xED || buffer[off + 1] != 0xED)
                        *dst++ = buffer[off++];
                    else {
                        off += 2;
                        uint8_t count = buffer[off++];
                        uint8_t byte = buffer[off++];
                        memset(dst, byte, count);
                        dst += count;
                    }
                }
            }
        }
    }
}

#endif
