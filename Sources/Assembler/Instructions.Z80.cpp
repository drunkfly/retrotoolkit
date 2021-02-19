#include "Instructions.Z80.h"

bool Z80::Opcode::isZ80Opcode() const { return true; }

#define Z80_OPCODE_0(OP, BYTES, TSTATES)
#define Z80_OPCODE_1(OP, OP1, BYTES, TSTATES)
#define Z80_OPCODE_2(OP, OP1, OP2, BYTES, TSTATES)

#include "Instructions.Z80.hh"
