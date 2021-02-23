#ifndef COMPILER_LINKER_CODEEMITTER_H
#define COMPILER_LINKER_CODEEMITTER_H

#include "Common/Common.h"

class CodeEmitter
{
public:
    virtual ~CodeEmitter() = default;
    virtual void write(const void* data, size_t size) = 0;
};

#endif
