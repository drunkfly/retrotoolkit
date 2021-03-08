#ifndef COMPILER_COMPRESSION_COMPRESSOR_H
#define COMPILER_COMPRESSION_COMPRESSOR_H

#include "Compiler/Compression/Compression.h"
#include <vector>
#include <memory>

class SourceLocation;

class Compressor
{
public:
    virtual ~Compressor() = default;
    virtual Compression compression() const = 0;
    virtual void compress(SourceLocation* location, std::vector<uint8_t> src, std::vector<uint8_t>& dst) = 0;

    static std::unique_ptr<Compressor> create(SourceLocation* location, Compression compression);
};

#endif
