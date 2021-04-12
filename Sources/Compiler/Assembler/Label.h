#ifndef COMPILER_ASSEMBLER_LABEL_H
#define COMPILER_ASSEMBLER_LABEL_H

#include "Compiler/Assembler/Instruction.h"

class Label final : public Instruction
{
public:
    class Address;

    Label(SourceLocation* location, const char* name);
    ~Label() override;

    Type type() const final override;

    const char* name() const { return mName; }

    bool calculateSizeInBytes(size_t& outSize,
        ISectionResolver* sectionResolver, std::unique_ptr<CompilerError>& resolveError) const override;
    bool canEmitCodeWithoutBaseAddress(ISectionResolver* sectionResolver) const override;
    bool emitCode(CodeEmitter* emitter, int64_t& nextAddress, ISectionResolver* sectionResolver,
        std::unique_ptr<CompilerError>& resolveError) const override;

    bool hasAddress() const;
    Address* address() const;
    size_t addressValue() const;

    void setAddress(size_t address);
    void unsetAddresses();

    void resetCounters() const final override;
    void saveReadCounter() const final override;
    void restoreReadCounter() const final override;
    void advanceCounters() const final override;

    Instruction* clone() const override;

private:
    const char* mName;
    mutable Address* mFirstAddress;
    mutable Address* mCurrentReadAddress;
    mutable Address* mCurrentWriteAddress;
    mutable Address* mSavedReadAddress;

    DISABLE_COPY(Label);
};

#endif
