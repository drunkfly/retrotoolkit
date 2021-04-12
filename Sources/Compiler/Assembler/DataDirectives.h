#ifndef COMPILER_ASSEMBLER_DATADIRECTIVES_H
#define COMPILER_ASSEMBLER_DATADIRECTIVES_H

#include "Compiler/Assembler/Instruction.h"

class Expr;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DEFB final : public Instruction
{
public:
    DEFB(SourceLocation* location, Expr* value);
    ~DEFB() override;

    bool calculateSizeInBytes(size_t& outSize,
        ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const override;
    bool canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const override;
    bool emitCode(CodeEmitter* emitter, int64_t& nextAddress, ISectionResolver* sectionResolver,
        std::unique_ptr<CompilerError>& resolveError) const override;

    Instruction* clone() const override;

private:
    Expr* mValue;

    DISABLE_COPY(DEFB);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DEFB_STRING final : public Instruction
{
public:
    DEFB_STRING(SourceLocation* location, const char* text, size_t length);
    ~DEFB_STRING() override;

    bool calculateSizeInBytes(size_t& outSize,
        ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const override;
    bool canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const override;
    bool emitCode(CodeEmitter* emitter, int64_t& nextAddress, ISectionResolver* sectionResolver,
        std::unique_ptr<CompilerError>& resolveError) const override;

    Instruction* clone() const override;

private:
    const char* mText;
    size_t mLength;

    DISABLE_COPY(DEFB_STRING);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DEFW final : public Instruction
{
public:
    DEFW(SourceLocation* location, Expr* value);
    ~DEFW() override;

    bool calculateSizeInBytes(size_t& outSize,
        ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const override;
    bool canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const override;
    bool emitCode(CodeEmitter* emitter, int64_t& nextAddress, ISectionResolver* sectionResolver,
        std::unique_ptr<CompilerError>& resolveError) const override;

    Instruction* clone() const override;

private:
    Expr* mValue;

    DISABLE_COPY(DEFW);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DEFD final : public Instruction
{
public:
    DEFD(SourceLocation* location, Expr* value);
    ~DEFD() override;

    bool calculateSizeInBytes(size_t& outSize,
        ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const override;
    bool canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const override;
    bool emitCode(CodeEmitter* emitter, int64_t& nextAddress, ISectionResolver* sectionResolver,
        std::unique_ptr<CompilerError>& resolveError) const override;

    Instruction* clone() const override;

private:
    Expr* mValue;

    DISABLE_COPY(DEFD);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DEFS final : public Instruction
{
public:
    DEFS(SourceLocation* location, Expr* value);
    ~DEFS() override;

    bool calculateSizeInBytes(size_t& outSize,
        ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const override;
    bool canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const override;
    bool emitCode(CodeEmitter* emitter, int64_t& nextAddress, ISectionResolver* sectionResolver,
        std::unique_ptr<CompilerError>& resolveError) const override;

    Instruction* clone() const override;

private:
    Expr* mValue;
    mutable std::optional<size_t> mSize;

    DISABLE_COPY(DEFS);
};

#endif
