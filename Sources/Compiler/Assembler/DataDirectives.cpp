#include "DataDirectives.h"
#include "Compiler/CompilerError.h"
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

bool DEFB::calculateSizeInBytes(size_t& outSize, std::unique_ptr<CompilerError>&) const
{
    outSize = 1;
    return true;
}

bool DEFB::canEmitCodeWithoutBaseAddress() const
{
    std::unique_ptr<CompilerError> resolveError;
    return mValue->canEvaluateValue(nullptr, resolveError);
}

bool DEFB::emitCode(CodeEmitter* emitter, int64_t& nextAddress, std::unique_ptr<CompilerError>& resolveError) const
{
    if (!mValue->canEvaluateValue(&nextAddress, resolveError))
        return false;
    emitter->emitByte(location(), mValue->evaluateByte(&nextAddress));
    nextAddress++;
    return true;
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

bool DEFB_STRING::calculateSizeInBytes(size_t& outSize, std::unique_ptr<CompilerError>&) const
{
    outSize = mLength;
    return true;
}

bool DEFB_STRING::canEmitCodeWithoutBaseAddress() const
{
    return true;
}

bool DEFB_STRING::emitCode(CodeEmitter* emitter, int64_t& nextAddress, std::unique_ptr<CompilerError>&) const
{
    emitter->emitBytes(location(), reinterpret_cast<const uint8_t*>(mText), mLength);
    nextAddress += mLength;
    return true;
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

bool DEFW::calculateSizeInBytes(size_t& outSize, std::unique_ptr<CompilerError>&) const
{
    outSize = 2;
    return true;
}

bool DEFW::canEmitCodeWithoutBaseAddress() const
{
    std::unique_ptr<CompilerError> resolveError;
    return mValue->canEvaluateValue(nullptr, resolveError);
}

bool DEFW::emitCode(CodeEmitter* emitter, int64_t& nextAddress, std::unique_ptr<CompilerError>& resolveError) const
{
    if (!mValue->canEvaluateValue(&nextAddress, resolveError))
        return false;

    uint16_t value = mValue->evaluateWord(&nextAddress);
    uint8_t bytes[] = { uint8_t(value & 0xff), uint8_t((value >> 8) & 0xff) };
    emitter->emitBytes(location(), bytes, 2);

    nextAddress += 2;
    return true;
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

bool DEFD::calculateSizeInBytes(size_t& outSize, std::unique_ptr<CompilerError>&) const
{
    outSize = 4;
    return true;
}

bool DEFD::canEmitCodeWithoutBaseAddress() const
{
    std::unique_ptr<CompilerError> resolveError;
    return mValue->canEvaluateValue(nullptr, resolveError);
}

bool DEFD::emitCode(CodeEmitter* emitter, int64_t& nextAddress, std::unique_ptr<CompilerError>& resolveError) const
{
    if (!mValue->canEvaluateValue(&nextAddress, resolveError))
        return false;

    uint32_t value = mValue->evaluateDWord(&nextAddress);
    uint8_t bytes[] = {
            uint8_t(value & 0xff),
            uint8_t((value >> 8) & 0xff),
            uint8_t((value >> 16) & 0xff),
            uint8_t((value >> 24) & 0xff),
        };
    emitter->emitBytes(location(), bytes, 4);

    nextAddress += 4;
    return true;
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

bool DEFS::calculateSizeInBytes(size_t& outSize, std::unique_ptr<CompilerError>& resolveError) const
{
    if (!mValue->canEvaluateValue(nullptr, resolveError))
        return false;
    mSize = mValue->evaluateUnsignedWord(nullptr);
    outSize = *mSize;
    return true;
}

bool DEFS::canEmitCodeWithoutBaseAddress() const
{
    std::unique_ptr<CompilerError> resolveError;
    return mSize.has_value() || mValue->canEvaluateValue(nullptr, resolveError);
}

bool DEFS::emitCode(CodeEmitter* emitter, int64_t& nextAddress, std::unique_ptr<CompilerError>&) const
{
    size_t n = mSize.value();
    for (size_t i = 0; i < n; i++)
        emitter->emitByte(location(), 0);
    nextAddress += n;
    return true;
}
