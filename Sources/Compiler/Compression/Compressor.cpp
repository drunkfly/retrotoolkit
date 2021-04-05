#include "Compressor.h"
#include "Compiler/CompilerError.h"
#include "Compiler/Compression/Lzsa2Compressor.h"
#include "Compiler/Compression/Zx0Compressor.h"
#include "Compiler/Compression/Zx7Compressor.h"

std::unique_ptr<Compressor> Compressor::create(SourceLocation* location, Compression compression)
{
    switch (compression) {
        case Compression::None:
            throw CompilerError(location, "internal compiler error: attempted to create dummy compressor.");
        case Compression::Lzsa2:
            return std::unique_ptr<Compressor>(new Lzsa2Compressor);
        case Compression::Zx7:
            return std::unique_ptr<Compressor>(new Zx7Compressor);
        case Compression::Zx0:
            return std::unique_ptr<Compressor>(new Zx0Compressor(false));
        case Compression::Zx0Quick:
            return std::unique_ptr<Compressor>(new Zx0Compressor(true));
    }

    throw CompilerError(location, "internal compiler error: invalid compression mode.");
}
