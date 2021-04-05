#include "Zx0Compressor.h"
#include "Compiler/CompilerError.h"

extern "C" {
#include <zx0.h>
}

Zx0Compressor::Zx0Compressor(bool quick)
    : mQuick(quick)
{
}

Zx0Compressor::~Zx0Compressor()
{
}

Compression Zx0Compressor::compression() const
{
    return (mQuick ? Compression::Zx0Quick : Compression::Zx0);
}

void Zx0Compressor::compress(SourceLocation* location, std::vector<uint8_t> src, std::vector<uint8_t>& dst)
{
    ZX0Context* context = nullptr;
    ZX0Block** optimal = nullptr;
    unsigned char* compressed = nullptr;
    int compressedSize = 0;

    if (src.empty())
        return;

    try {
        context = zx0_new();
        if (!context)
            throw CompilerError(location, "zx0: out of memory.");

        optimal = zx0_optimize(context, src.data(), src.size(), 0, (mQuick ? 2176 : 32640));
        if (!optimal)
            throw CompilerError(location, "zx0: out of memory.");

        int delta = 0;
        compressed = zx0_compress(optimal[src.size() - 1], src.data(), src.size(), 0, FALSE, &compressedSize, &delta);
        if (!compressed)
            throw CompilerError(location, "zx0: out of memory.");

        dst.reserve(dst.size() + compressedSize);
        dst.insert(dst.end(), compressed, compressed + compressedSize);
    } catch (...) {
        if (compressed)
            free(compressed);
        if (optimal)
            free(optimal);
        if (context)
            zx0_delete(context);
        throw;
    }

    free(compressed);
    free(optimal);
    zx0_delete(context);
}
