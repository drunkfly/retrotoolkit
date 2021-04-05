#ifndef COMPILER_ASSEMBLER_LABEL_H
#define COMPILER_ASSEMBLER_LABEL_H

#include "Compiler/Assembler/Instruction.h"

class Label : public Instruction
{
public:
    class Address;

    Label(SourceLocation* location, const char* name);
    ~Label() override;

    Type type() const final override;

    const char* name() const { return mName; }

    bool calculateSizeInBytes(size_t& outSize, std::unique_ptr<CompilerError>& resolveError) const override;
    bool canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const override;
    bool emitCode(CodeEmitter* emitter, int64_t& nextAddress, ISectionResolver* sectionResolver,
        std::unique_ptr<CompilerError>& resolveError) const override;

    bool hasAddress() const;
    Address* address() const;
    size_t addressValue() const;

    void setAddress(size_t address);
    void unsetAddress();

private:
    class SimpleAddress;

    const char* mName;
    Address* mAddress;

    DISABLE_COPY(Label);
};

#endif
