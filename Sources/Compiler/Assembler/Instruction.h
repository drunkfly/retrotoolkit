#ifndef COMPILER_ASSEMBLER_INSTRUCTION_H
#define COMPILER_ASSEMBLER_INSTRUCTION_H

#include "Common/Common.h"
#include "Common/GC.h"
#include "Compiler/Tree/SourceLocation.h"

class Instruction : public GCObject
{
public:
    explicit Instruction(SourceLocation* location)
        : mLocation(location)
    {
    }

    virtual bool isLabel() const;
    virtual bool isZ80Opcode() const;

    virtual size_t sizeInBytes() const = 0;

    SourceLocation* location() const { return mLocation; }

private:
    SourceLocation* mLocation;

    DISABLE_COPY(Instruction);
};

#endif
