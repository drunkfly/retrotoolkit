#ifndef EMULATOR_EMULATOR_H
#define EMULATOR_EMULATOR_H

#include "Emulator/Common.h"

class Emulator
{
public:
    Emulator();
    ~Emulator();

    Z80Cpu* cpu() const { return mCpu.get(); }
    Z80Memory* memory() const { return mMemory.get(); }
    Z80Screen* screen() const { return mScreen.get(); }

    void loadSnapshot(const SnapshotState* state, const uint8_t* memory);

    void runFrame();

private:
    std::unique_ptr<Z80Memory> mMemory;
    std::unique_ptr<Z80Screen> mScreen;
    std::unique_ptr<Z80Cpu> mCpu;

    DISABLE_COPY(Emulator);
};

#endif
