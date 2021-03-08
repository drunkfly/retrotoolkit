#ifndef COMPILER_COMPRESSION_ZX7COMPRESSOR_H
#define COMPILER_COMPRESSION_ZX7COMPRESSOR_H

#include "Common/Common.h"
#include "Compiler/Compression/Compressor.h"

class Zx7Compressor final : public Compressor
{
public:
    Zx7Compressor();
    ~Zx7Compressor() override;

    Compression compression() const override;
    void compress(SourceLocation* location, std::vector<uint8_t> src, std::vector<uint8_t>& dst) override;

    DISABLE_COPY(Zx7Compressor);
};

#endif
