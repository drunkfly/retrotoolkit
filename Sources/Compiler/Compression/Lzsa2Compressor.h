#ifndef COMPILER_COMPRESSION_LZSA2COMPRESSOR_H
#define COMPILER_COMPRESSION_LZSA2COMPRESSOR_H

#include "Common/Common.h"
#include "Compiler/Compression/Compressor.h"

class Lzsa2Compressor : public Compressor
{
public:
    Lzsa2Compressor();
    ~Lzsa2Compressor() override;

    void compress(SourceLocation* location, std::vector<uint8_t> src, std::vector<uint8_t>& dst) override;

    DISABLE_COPY(Lzsa2Compressor);
};

#endif
