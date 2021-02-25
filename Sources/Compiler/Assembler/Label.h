#ifndef COMPILER_ASSEMBLER_LABEL_H
#define COMPILER_ASSEMBLER_LABEL_H

#include "Compiler/Assembler/Instruction.h"

class Label : public Instruction
{
public:
    class Address;

    Label(SourceLocation* location, std::string name);
    ~Label() override;

    bool isLabel() const final override;

    const std::string& name() const { return mName; }

    size_t sizeInBytes() const override;

    bool hasAddress() const;
    Address* address() const;

    void setAddress(size_t address);
    void unsetAddress();

private:
    class SimpleAddress;

    std::string mName;
    Address* mAddress;

    DISABLE_COPY(Label);
};

#endif
