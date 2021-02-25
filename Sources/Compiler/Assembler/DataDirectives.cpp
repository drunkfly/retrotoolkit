#include "DataDirectives.h"
#include "Compiler/Linker/CodeEmitter.h"
#include "Compiler/Tree/Expr.h"
#include <string.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DEFB::DEFB(SourceLocation* location, Expr* value)
    : Instruction(location)
    , mValue(value)
{
}

DEFB::~DEFB()
{
}

size_t DEFB::sizeInBytes() const
{
    return 1;
}

void DEFB::emitCode(CodeEmitter* emitter, int64_t& nextAddress) const
{
    emitter->emitByte(location(), mValue->evaluateByte());
    nextAddress++;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DEFB_STRING::DEFB_STRING(SourceLocation* location, const char* text)
    : Instruction(location)
    , mText(text)
    , mLength(strlen(text))
{
}

DEFB_STRING::~DEFB_STRING()
{
}

size_t DEFB_STRING::sizeInBytes() const
{
    return mLength;
}

void DEFB_STRING::emitCode(CodeEmitter* emitter, int64_t& nextAddress) const
{
    emitter->emitBytes(location(), reinterpret_cast<const uint8_t*>(mText), mLength);
    nextAddress += mLength;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DEFW::DEFW(SourceLocation* location, Expr* value)
    : Instruction(location)
    , mValue(value)
{
}

DEFW::~DEFW()
{
}

size_t DEFW::sizeInBytes() const
{
    return 2;
}

void DEFW::emitCode(CodeEmitter* emitter, int64_t& nextAddress) const
{
    uint16_t value = mValue->evaluateWord();
    uint8_t bytes[] = {
            uint8_t(value & 0xff),
            uint8_t((value >> 8) & 0xff),
        };
    emitter->emitBytes(location(), bytes, 2);
    nextAddress += 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DEFD::DEFD(SourceLocation* location, Expr* value)
    : Instruction(location)
    , mValue(value)
{
}

DEFD::~DEFD()
{
}

size_t DEFD::sizeInBytes() const
{
    return 4;
}

void DEFD::emitCode(CodeEmitter* emitter, int64_t& nextAddress) const
{
    uint32_t value = mValue->evaluateDWord();
    uint8_t bytes[] = {
            uint8_t(value & 0xff),
            uint8_t((value >> 8) & 0xff),
            uint8_t((value >> 16) & 0xff),
            uint8_t((value >> 24) & 0xff),
        };
    emitter->emitBytes(location(), bytes, 4);
    nextAddress += 4;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DEFS::DEFS(SourceLocation* location, Expr* value)
    : Instruction(location)
    , mValue(value)
{
}

DEFS::~DEFS()
{
}

size_t DEFS::sizeInBytes() const
{
    if (!mSize)
        mSize = mValue->evaluateUnsignedWord();
    return *mSize;
}

void DEFS::emitCode(CodeEmitter* emitter, int64_t& nextAddress) const
{
    size_t n = mSize.value();
    for (size_t i = 0; i < n; i++)
        emitter->emitByte(location(), 0);
    nextAddress += n;
}
