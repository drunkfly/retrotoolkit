#ifndef COMPILER_LINKER_CODEEMITTER_H
#define COMPILER_LINKER_CODEEMITTER_H

#include "Common/Common.h"
#include <vector>
#include <stdint.h>

class SourceLocation;

class CodeEmitter
{
public:
    CodeEmitter();
    virtual ~CodeEmitter();

    virtual void emitByte(SourceLocation* location, uint8_t byte) = 0;

    DISABLE_COPY(CodeEmitter);
};

class UncompressedCodeEmitter : public CodeEmitter
{
public:
    struct Byte
    {
        SourceLocation* location;
        uint8_t value;
    };

    UncompressedCodeEmitter();
    ~UncompressedCodeEmitter();

    void clear();

    void emitByte(SourceLocation* location, uint8_t byte) override;

private:
    std::vector<Byte> mBytes;

    DISABLE_COPY(UncompressedCodeEmitter);
};

#endif
