#ifndef COMPILER_LINKER_CODEEMITTER_H
#define COMPILER_LINKER_CODEEMITTER_H

#include "Common/Common.h"

class SourceLocation;
enum class Compression;

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

    virtual void addEmptySpaceDebugInfo(int64_t start, int64_t size) = 0;
    virtual void addSectionDebugInfo(std::string name, int64_t start,
        Compression compression, int64_t uncompressedSize, std::optional<int64_t> compressedSize) = 0;

    virtual void emitByte(SourceLocation* location, uint8_t byte) = 0;
    virtual void emitBytes(SourceLocation* location, const uint8_t* bytes, size_t count) = 0;
    virtual void emitBytes(const Byte* bytes, size_t count) = 0;

    virtual void copyTo(CodeEmitter* target) const = 0;

    DISABLE_COPY(CodeEmitter);
};

#endif
