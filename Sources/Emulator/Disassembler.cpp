#include "Disassembler.h"
#include "Emulator/Emulator.h"
#include "Emulator/Z80Memory.h"
#include "Emulator/Z80Cpu.h"

Disassembler::Disassembler(Emulator* emulator)
    : mEmulator(emulator)
    , mAddress(mEmulator->cpu()->get_pc())
    , mStartAddress(mEmulator->cpu()->get_pc())
    , mMode(PrependAddress | AppendNewline)
{
    mBuffer[0] = 0;
    mBuffer[sizeof(mBuffer) - 1] = 0;
}

void Disassembler::on_emit(const char* out)
{
    switch (mMode) {
        case 0:
            snprintf(mBuffer, sizeof(mBuffer), "%s", out);
            break;

        case PrependAddress:
            snprintf(mBuffer, sizeof(mBuffer), "%04X %s", mStartAddress, out);
            break;

        case AppendNewline:
            snprintf(mBuffer, sizeof(mBuffer), "%s", out);
            break;

        case AppendNewline | PrependAddress:
            snprintf(mBuffer, sizeof(mBuffer), "%04X %s\n", mStartAddress, out);
            break;
    }
}

z80::fast_u8 Disassembler::on_read_next_byte()
{
    uint8_t value = mEmulator->memory()->read(mAddress);
    mAddress = (mAddress + 1) & 0xffff;
    return value;
}

z80::fast_u16 Disassembler::on_get_last_read_addr() const
{
    return (mAddress - 1) & 0xffff;
}

const char* Disassembler::on_disassemble(z80::fast_u16 addr)
{
    mAddress = addr;
    return on_disassemble();
}

const char* Disassembler::on_disassemble()
{
    mStartAddress = mAddress;

    base::on_disassemble();
    while (get_iregp_kind() != z80::iregp::hl)
        base::on_disassemble();

    return mBuffer;
}
