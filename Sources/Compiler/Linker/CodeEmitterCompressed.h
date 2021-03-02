#ifndef COMPILER_LINKER_CODEEMITTERCOMPRESSED_H
#define COMPILER_LINKER_CODEEMITTERCOMPRESSED_H

#include "Compiler/Linker/CodeEmitter.h"
#include <vector>

class Compressor;

class CodeEmitterCompressed : public CodeEmitter
{
public:
    explicit CodeEmitterCompressed(Compressor* compressor);
    ~CodeEmitterCompressed();

    size_t size() const;
    const uint8_t* data() const;

    void clear();

    void emitByte(SourceLocation* location, uint8_t byte) override;
    void emitBytes(SourceLocation* location, const uint8_t* bytes, size_t count) override;
    void emitBytes(const Byte* bytes, size_t count) override;

    void compress();

    void copyTo(CodeEmitter* target) const;

private:
    SourceLocation* mLocation;
    Compressor* mCompressor;
    std::vector<uint8_t> mCompressedBytes;
    std::vector<uint8_t> mUncompressedBytes;
    bool mCompressed;

    DISABLE_COPY(CodeEmitterCompressed);
};

#endif
