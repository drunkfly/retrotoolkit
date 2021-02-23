#ifndef COMPILER_LINKER_CODEEMITTER_H
#define COMPILER_LINKER_CODEEMITTER_H

#include "Common/Common.h"
#include <stdint.h>

class SourceLocation;

class CodeEmitter
{
public:
    virtual ~CodeEmitter() = default;
    virtual void emitByte(SourceLocation* location, uint8_t byte) = 0;
};

#endif
