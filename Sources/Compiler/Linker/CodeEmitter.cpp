#include "CodeEmitter.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CodeEmitter::CodeEmitter()
{
}

CodeEmitter::~CodeEmitter()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UncompressedCodeEmitter::UncompressedCodeEmitter()
{
}

UncompressedCodeEmitter::~UncompressedCodeEmitter()
{
}

void UncompressedCodeEmitter::clear()
{
    mBytes.clear();
}

void UncompressedCodeEmitter::emitByte(SourceLocation* location, uint8_t byte)
{
    mBytes.emplace_back(Byte{ location, byte });
}
