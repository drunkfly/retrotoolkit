#ifndef COMPILER_LINKER_CODEEMITTERUNCOMPRESSED_H
#define COMPILER_LINKER_CODEEMITTERUNCOMPRESSED_H

#include "Compiler/Linker/CodeEmitter.h"
#include <vector>

class CodeEmitterUncompressed : public CodeEmitter
{
public:
    CodeEmitterUncompressed();
    ~CodeEmitterUncompressed();

    size_t size() const { return mBytes.size(); }
    const Byte* data() const { return mBytes.data(); }

    void clear();

    void emitByte(SourceLocation* location, uint8_t byte) final override;
    void emitBytes(SourceLocation* location, const uint8_t* bytes, size_t count) final override;
    void emitBytes(const Byte* bytes, size_t count) final override;

    void copyTo(CodeEmitter* target) const final override;

private:
    std::vector<Byte> mBytes;

    DISABLE_COPY(CodeEmitterUncompressed);
};

#endif
