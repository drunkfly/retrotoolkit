#ifndef COMPILER_COMPRESSION_COMPRESSOR_H
#define COMPILER_COMPRESSION_COMPRESSOR_H

#include "Compiler/Compression/Compression.h"
#include <vector>

class SourceLocation;

class Compressor
{
public:
    virtual ~Compressor() = default;
    virtual void compress(SourceLocation* location, std::vector<uint8_t> src, std::vector<uint8_t>& dst) = 0;
};

#endif
