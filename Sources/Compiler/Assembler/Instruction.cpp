#include "Instruction.h"

bool Instruction::isLabel() const
{
    return false;
}

bool Instruction::isZ80Opcode() const
{
    return false;
}
