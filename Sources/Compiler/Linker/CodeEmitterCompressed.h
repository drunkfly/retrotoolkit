#ifndef COMPILER_LINKER_CODEEMITTERCOMPRESSED_H
#define COMPILER_LINKER_CODEEMITTERCOMPRESSED_H

#include "Compiler/Linker/CodeEmitter.h"
#include "Compiler/Linker/DebugInformation.h"

class Compressor;

class CodeEmitterCompressed final : public CodeEmitter
{
public:
    explicit CodeEmitterCompressed(std::unique_ptr<Compressor> compressor);
    ~CodeEmitterCompressed();

    size_t compressedSize() const;
    const uint8_t* compressedData() const;

    void clear();

    void addSectionDebugInfo(std::string name, int64_t start,
        Compression compression, int64_t uncompressedSize, std::optional<int64_t> compressedSize) override;

    void emitByte(SourceLocation* location, uint8_t byte) override;
    void emitBytes(SourceLocation* location, const uint8_t* bytes, size_t count) override;
    void emitBytes(const Byte* bytes, size_t count) override;

    void setSectionBase(int64_t base);

    void compress();

    void copyTo(CodeEmitter* target) const override;

private:
    SourceLocation* mLocation;
    std::unique_ptr<Compressor> mCompressor;
    std::unique_ptr<DebugInformation::Section> mSection;
    std::vector<uint8_t> mCompressedBytes;
    std::vector<uint8_t> mUncompressedBytes;
    bool mCompressed;

    DISABLE_COPY(CodeEmitterCompressed);
};

#endif
