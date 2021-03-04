#ifndef COMPILER_ASSEMBLER_ASSEMBLERCONTEXT_H
#define COMPILER_ASSEMBLER_ASSEMBLERCONTEXT_H

#include "Common/GC.h"
#include <string>

class ProgramSection;
class Instruction;
class SymbolTable;
class SourceLocation;
class Label;

class AssemblerContext : public GCObject
{
public:
    explicit AssemblerContext(AssemblerContext* prev);
    virtual ~AssemblerContext();

    AssemblerContext* prev() const { return mPrev; }

    virtual const std::string& localLabelsPrefix() const;
    virtual void setLocalLabelsPrefix(SourceLocation* location, std::string prefix);

    virtual bool setCurrentSection(ProgramSection* section);

    virtual void addInstruction(Instruction* instruction);
    virtual void addLabel(SymbolTable* symbolTable, SourceLocation* location, std::string name);
    virtual Label* addEphemeralLabel(SourceLocation* location);

private:
    AssemblerContext* mPrev;
    ProgramSection* mSection;
    std::string mLocalLabelsPrefix;
    int mEphemeralLabelCounter;

    DISABLE_COPY(AssemblerContext);
};

#endif
