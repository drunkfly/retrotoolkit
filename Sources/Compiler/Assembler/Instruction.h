#ifndef COMPILER_ASSEMBLER_INSTRUCTION_H
#define COMPILER_ASSEMBLER_INSTRUCTION_H

#include "Common/Common.h"
#include "Common/GC.h"
#include "Compiler/Tree/SourceLocation.h"

class CodeEmitter;

class Instruction : public GCObject
{
public:
    explicit Instruction(SourceLocation* location)
        : mLocation(location)
    {
    }

    virtual bool isLabel() const;
    virtual bool isZ80Opcode() const;

    SourceLocation* location() const { return mLocation; }

    virtual size_t sizeInBytes() const = 0;
    virtual void emitCode(CodeEmitter* emitter, int64_t& nextAddress) const = 0;

private:
    SourceLocation* mLocation;

    DISABLE_COPY(Instruction);
};

#endif
