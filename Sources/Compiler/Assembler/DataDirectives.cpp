#include "DataDirectives.h"
#include "Compiler/CompilerError.h"
#include "Compiler/Linker/CodeEmitter.h"
#include "Compiler/Tree/Expr.h"

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

bool DEFB::canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const
{
    std::unique_ptr<CompilerError> resolveError;
    return mValue->canEvaluateValue(nullptr, sectionResolver, resolveError);
}

bool DEFB::emitCode(CodeEmitter* emitter,
    int64_t& nextAddress, ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const
{
    if (!mValue->canEvaluateValue(&nextAddress, sectionResolver, resolveError))
        return false;
    emitter->emitByte(location(), mValue->evaluateByte(&nextAddress, sectionResolver));
    nextAddress++;
    return true;
}

Instruction* DEFB::clone() const
{
    return new (heap()) DEFB(location(), mValue);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DEFB_STRING::DEFB_STRING(SourceLocation* location, const char* text, size_t length)
    : Instruction(location)
    , mText(text)
    , mLength(length)
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

bool DEFB_STRING::canEmitCodeWithoutBaseAddress(ISectionResolver*) const
{
    return true;
}

bool DEFB_STRING::emitCode(CodeEmitter* emitter, int64_t& nextAddress,
    ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>&) const
{
    emitter->emitBytes(location(), reinterpret_cast<const uint8_t*>(mText), mLength);
    nextAddress += mLength;
    return true;
}

Instruction* DEFB_STRING::clone() const
{
    return new (heap()) DEFB_STRING(location(), mText, mLength);
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

bool DEFW::canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const
{
    std::unique_ptr<CompilerError> resolveError;
    return mValue->canEvaluateValue(nullptr, sectionResolver, resolveError);
}

bool DEFW::emitCode(CodeEmitter* emitter, int64_t& nextAddress,
    ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const
{
    if (!mValue->canEvaluateValue(&nextAddress, sectionResolver, resolveError))
        return false;

    uint16_t value = mValue->evaluateWord(&nextAddress, sectionResolver);
    uint8_t bytes[] = { uint8_t(value & 0xff), uint8_t((value >> 8) & 0xff) };
    emitter->emitBytes(location(), bytes, 2);

    nextAddress += 2;
    return true;
}

Instruction* DEFW::clone() const
{
    return new (heap()) DEFW(location(), mValue);
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

bool DEFD::canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const
{
    std::unique_ptr<CompilerError> resolveError;
    return mValue->canEvaluateValue(nullptr, sectionResolver, resolveError);
}

bool DEFD::emitCode(CodeEmitter* emitter, int64_t& nextAddress,
    ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const
{
    if (!mValue->canEvaluateValue(&nextAddress, sectionResolver, resolveError))
        return false;

    uint32_t value = mValue->evaluateDWord(&nextAddress, sectionResolver);
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

Instruction* DEFD::clone() const
{
    return new (heap()) DEFD(location(), mValue);
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
    if (!mValue->canEvaluateValue(nullptr, nullptr, resolveError))
        return false;
    mSize = mValue->evaluateUnsignedWord(nullptr, nullptr);
    outSize = *mSize;
    return true;
}

bool DEFS::canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const
{
    std::unique_ptr<CompilerError> resolveError;
    return mSize.has_value() || mValue->canEvaluateValue(nullptr, sectionResolver, resolveError);
}

bool DEFS::emitCode(CodeEmitter* emitter, int64_t& nextAddress,
    ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>&) const
{
    size_t n = mSize.value();
    for (size_t i = 0; i < n; i++)
        emitter->emitByte(location(), 0);
    nextAddress += n;
    return true;
}

Instruction* DEFS::clone() const
{
    return new (heap()) DEFS(location(), mValue);
}
