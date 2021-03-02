#ifndef COMPILER_COMPRESSION_LZSACOMPRESSOR_H
#define COMPILER_COMPRESSION_LZSACOMPRESSOR_H

#include "Common/Common.h"
#include "Compiler/Compression/Compressor.h"

class LzsaCompressor : public Compressor
{
public:
    LzsaCompressor();
    ~LzsaCompressor() override;

    void compress(SourceLocation* location, std::vector<uint8_t> src, std::vector<uint8_t>& dst) override;

    DISABLE_COPY(LzsaCompressor);
};

#endif
