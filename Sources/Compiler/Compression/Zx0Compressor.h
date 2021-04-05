#ifndef COMPILER_COMPRESSION_ZX0COMPRESSOR_H
#define COMPILER_COMPRESSION_ZX0COMPRESSOR_H

#include "Compiler/Compression/Compressor.h"

class Zx0Compressor final : public Compressor
{
public:
    explicit Zx0Compressor(bool quick);
    ~Zx0Compressor() override;

    Compression compression() const override;
    void compress(SourceLocation* location, std::vector<uint8_t> src, std::vector<uint8_t>& dst) override;

private:
    bool mQuick;

    DISABLE_COPY(Zx0Compressor);
};

#endif
