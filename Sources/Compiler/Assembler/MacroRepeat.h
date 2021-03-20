#ifndef COMPILER_ASSEMBLER_MACROREPEAT_H
#define COMPILER_ASSEMBLER_MACROREPEAT_H

#include "Compiler/Assembler/Instruction.h"
#include <vector>

class MacroRepeat final : public Instruction
{
public:
    explicit MacroRepeat(SourceLocation* location)
        : Instruction(location)
    {
        registerFinalizer();
    }

    Type type() const final override;

    void addInstruction(Instruction* instruction);

    bool resolveLabels(size_t& address, std::unique_ptr<CompilerError>& resolveError);
    void unresolveLabels();

    bool calculateSizeInBytes(size_t& outSize, std::unique_ptr<CompilerError>& resolveError) const final override;
    bool canEmitCodeWithoutBaseAddress() const final override;
    bool emitCode(CodeEmitter* emitter, int64_t& nextAddress,
        std::unique_ptr<CompilerError>& resolveError) const final override;

private:
    std::vector<Instruction*> mInstructions;

    DISABLE_COPY(MacroRepeat);
};

#endif
