#ifndef COMPILER_ASSEMBLER_INSTRUCTION_H
#define COMPILER_ASSEMBLER_INSTRUCTION_H

#include "Common/Common.h"
#include "Common/GC.h"
#include "Compiler/Tree/SourceLocation.h"

class Instruction : public GCObject
{
public:
    Instruction(SourceLocation* location)
        : mLocation(location)
    {
    }

    virtual bool isZ80Opcode() const;

    virtual unsigned char sizeInBytes() const = 0;

    SourceLocation* location() const { return mLocation; }

private:
    SourceLocation* mLocation;

    DISABLE_COPY(Instruction);
};

#endif
