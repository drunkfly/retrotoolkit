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

    void emitByte(SourceLocation* location, uint8_t byte) override;
    void emitBytes(SourceLocation* location, const uint8_t* bytes, size_t count) override;
    void emitBytes(const Byte* bytes, size_t count) override;

    void copyTo(CodeEmitter* target) const;

private:
    std::vector<Byte> mBytes;

    DISABLE_COPY(CodeEmitterUncompressed);
};

#endif
