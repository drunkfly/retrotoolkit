#include "Zx7Compressor.h"
#include "Compiler/CompilerError.h"

extern "C" {
#include <zx7.h>
}

void Zx7Compressor::compress(SourceLocation* location, std::vector<uint8_t> src, std::vector<uint8_t>& dst)
{
    Optimal* optimal = nullptr;
    unsigned char* compressed = nullptr;
    size_t compressedSize = 0;

    if (src.empty())
        return;

    try {
        optimal = zx7_optimize(src.data(), src.size(), 0);
        if (!optimal)
            throw CompilerError(location, "zx7: out of memory.");

        long delta = 0;
        compressed = zx7_compress(optimal, src.data(), src.size(), 0, &compressedSize, &delta);
        if (!compressed)
            throw CompilerError(location, "zx7: out of memory.");

        dst.reserve(dst.size() + compressedSize);
        dst.insert(dst.end(), compressed, compressed + compressedSize);
    } catch (...) {
        if (compressed)
            free(compressed);
        if (optimal)
            free(optimal);
        throw;
    }

    free(compressed);
    free(optimal);
}
