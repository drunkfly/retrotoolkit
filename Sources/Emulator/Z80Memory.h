#ifndef EMULATOR_Z80MEMORY_H
#define EMULATOR_Z80MEMORY_H

#include "Emulator/Common.h"

class Z80Memory : public z80::z80_cpu<Z80Memory>
{
public:
    enum
    {
        BankCount = 8,
        BankSize = 16384,
    };

    explicit Z80Memory(Emulator* emulator);

    uint8_t* bytes() { return mMemory; }
    const uint8_t* bytes() const { return mMemory; }

    uint8_t* bankData(int bank) { return &mMemory[bank * BankSize]; }
    const uint8_t* bankData(int bank) const { return &mMemory[bank * BankSize]; }

    int currentBank() const { return mCurrentBank; }
    void setCurrentBank(int bank) { mCurrentBank = bank; }

    static size_t resolveAddress(int bank, uint16_t addr);

    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t value);

private:
    Emulator* mEmulator;
    int mCurrentBank;
    uint8_t mMemory[BankCount * BankSize];

    DISABLE_COPY(Z80Memory);
};

#endif
