#include "Emulator.h"
#include "Emulator/Z80Memory.h"
#include "Emulator/Z80Screen.h"
#include "Emulator/Z80Cpu.h"
#include "Emulator/Snapshot.h"

Emulator::Emulator()
{
    mMemory = std::make_unique<Z80Memory>(this);
    mScreen = std::make_unique<Z80Screen>(this);
    mCpu = std::make_unique<Z80Cpu>(this);
}

Emulator::~Emulator()
{
}

void Emulator::loadSnapshot(const SnapshotState* state, const uint8_t* memory)
{
    mCpu->set_a(state->a);
    mCpu->set_f(state->f);
    mCpu->set_b(state->b);
    mCpu->set_c(state->c);
    mCpu->set_d(state->d);
    mCpu->set_e(state->e);
    mCpu->set_h(state->h);
    mCpu->set_l(state->l);
    mCpu->set_alt_af((state->shadowA << 8) | state->shadowF);
    mCpu->set_alt_bc((state->shadowB << 8) | state->shadowC);
    mCpu->set_alt_de((state->shadowD << 8) | state->shadowE);
    mCpu->set_alt_hl((state->shadowH << 8) | state->shadowL);
    mCpu->set_ix((state->ixH << 8) | state->ixL);
    mCpu->set_iy((state->iyH << 8) | state->iyL);
    mCpu->set_i(state->i);
    mCpu->set_r(state->r);
    mCpu->set_int_mode(state->intMode);
    mCpu->set_is_int_disabled(state->intDisabled != 0);
    mCpu->set_pc((state->pcH << 8) | state->pcL);
    mCpu->set_sp((state->spH << 8) | state->spL);
    mCpu->on_output(0x1FFD, state->port1FFD);
    mCpu->on_output(0x7FFD, state->port7FFD);
    mCpu->on_output(0xFFFD, state->portFFFD);
    mScreen->setBorderColor(state->borderColor);
    memcpy(mMemory->bytes(), memory, Z80Memory::BankCount * Z80Memory::BankSize);
}

void Emulator::runFrame()
{
    mCpu->run();
}
