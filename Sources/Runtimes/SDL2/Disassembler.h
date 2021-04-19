#ifndef RUNTIMES_SDL2_DISASSEMBLER_H
#define RUNTIMES_SDL2_DISASSEMBLER_H

#include "Runtimes/SDL2/Common.h"

class Cpu;

class Disassembler : public z80::z80_disasm<Disassembler>
{
public:
    explicit Disassembler(Cpu* cpu);

    void setAppendNewline(bool flag) { mMode = (flag ? mMode | AppendNewline : mMode & ~AppendNewline); }
    void setPrependAddress(bool flag) { mMode = (flag ? mMode | PrependAddress : mMode & ~PrependAddress); }

    void on_emit(const char* out);

    z80::fast_u8 on_read_next_byte();
    z80::fast_u16 on_get_last_read_addr() const;

    const char* on_disassemble(z80::fast_u16 addr);
    const char* on_disassemble();

private:
    enum
    {
        AppendNewline = 0x01,
        PrependAddress = 0x02,
    };

    Cpu* mCpu;
    z80::fast_u16 mAddress;
    z80::fast_u16 mStartAddress;
    char mBuffer[64];
    uint8_t mMode;

    DISABLE_COPY(Disassembler);
};

#endif
