#include "Instruction.h"
#include "Compiler/Assembler/MacroIf.h"
#include "Compiler/Assembler/MacroRepeat.h"
#include "Compiler/Assembler/Label.h"
#include "Compiler/CompilerError.h"

Instruction::Type Instruction::type() const
{
    return Type::Default;
}

bool Instruction::isZ80Opcode() const
{
    return false;
}

bool Instruction::resolveLabel(size_t& address, std::unique_ptr<CompilerError>& resolveError)
{
    if (address > 0xffff)
        throw CompilerError(location(), "address is over 64K.");

    switch (type()) {
        case Type::Label:
            static_cast<Label*>(this)->setAddress(address);
            break;

        case Type::If:
            if (!static_cast<MacroIf*>(this)->resolveLabels(address, resolveError))
                return false;
            break;

        case Type::Repeat:
            if (!static_cast<MacroRepeat*>(this)->resolveLabels(address, resolveError))
                return false;
            break;

        case Type::Default: {
            size_t size;
            if (!calculateSizeInBytes(size, resolveError))
                return false;
            address += size;
        }
    }

    return true;
}

void Instruction::unresolveLabel()
{
    switch (type()) {
        case Type::Label:
            static_cast<Label*>(this)->unsetAddress();
            break;

        case Type::If:
            static_cast<MacroIf*>(this)->unresolveLabels();
            break;

        case Type::Repeat:
            static_cast<MacroRepeat*>(this)->unresolveLabels();
            break;

        case Type::Default:
            break;
    }
}
