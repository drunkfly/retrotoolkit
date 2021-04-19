#include "Z80Memory.h"

Z80Memory::Z80Memory(Emulator* emulator)
    : mEmulator(emulator)
    , mCurrentBank(0)
{
    memset(mMemory, 0, sizeof(mMemory));
}

size_t Z80Memory::resolveAddress(int bank, uint16_t addr)
{
    if (addr < 0x4000) {
        assert(false);
        throw std::runtime_error("attempted to access ROM memory.");
    }

    if (addr < 0x8000)
        return addr - 0x4000 + BankSize * 5;
    else if (addr < 0xc000)
        return addr - 0x8000 + BankSize * 2;
    else
        return addr - 0xc000 + BankSize * bank;
}

uint8_t Z80Memory::read(uint16_t address)
{
    return mMemory[resolveAddress(mCurrentBank, address)];
}

void Z80Memory::write(uint16_t address, uint8_t value)
{
    mMemory[resolveAddress(mCurrentBank, address)] = value;
}
