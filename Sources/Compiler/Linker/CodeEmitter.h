#ifndef COMPILER_LINKER_CODEEMITTER_H
#define COMPILER_LINKER_CODEEMITTER_H

#include "Common/Common.h"
#include <vector>
#include <stdint.h>

class SourceLocation;

class CodeEmitter
{
public:
    struct Byte
    {
        SourceLocation* location;
        uint8_t value;
    };

    CodeEmitter();
    virtual ~CodeEmitter();

    virtual void emitByte(SourceLocation* location, uint8_t byte) = 0;
    virtual void emitBytes(SourceLocation* location, const uint8_t* bytes, size_t count) = 0;
    virtual void emitBytes(const Byte* bytes, size_t count) = 0;

    DISABLE_COPY(CodeEmitter);
};

class UncompressedCodeEmitter : public CodeEmitter
{
public:
    UncompressedCodeEmitter();
    ~UncompressedCodeEmitter();

    size_t size() const { return mBytes.size(); }
    const Byte* data() const { return mBytes.data(); }

    void clear();

    void emitByte(SourceLocation* location, uint8_t byte) override;
    void emitBytes(SourceLocation* location, const uint8_t* bytes, size_t count) override;
    void emitBytes(const Byte* bytes, size_t count) override;

    void copyTo(CodeEmitter* target) const;

private:
    std::vector<Byte> mBytes;

    DISABLE_COPY(UncompressedCodeEmitter);
};

#endif
