#ifndef ASSEMBLER_INSTRUCTION_H
#define ASSEMBLER_INSTRUCTION_H

#include "Common/Common.h"
#include "Common/SourceLocation.h"

class Instruction
{
public:
    Instruction() = default;
    virtual ~Instruction();

    virtual bool isZ80Opcode() const;

    const SourceLocation& location() const { return mLocation; }

private:
    SourceLocation mLocation;

    DISABLE_COPY(Instruction);
};

#endif
