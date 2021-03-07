#ifndef COMPILER_ASSEMBLER_ASSEMBLERCONTEXT_H
#define COMPILER_ASSEMBLER_ASSEMBLERCONTEXT_H

#include "Common/GC.h"
#include <string>

class Expr;
class ProgramSection;
class Instruction;
class SymbolTable;
class SourceLocation;
class Label;
class Token;

class AssemblerContext : public GCObject
{
public:
    explicit AssemblerContext(AssemblerContext* prev);
    virtual ~AssemblerContext();

    AssemblerContext* prev() const { return mPrev; }

    virtual bool isIf() const;
    virtual bool hasElse() const;
    virtual void beginElse(const Token* token);

    virtual bool isRepeat() const;
    virtual bool hasVariable(const std::string& name) const;

    virtual void validateAtPop(SourceLocation* location);

    virtual const std::string& localLabelsPrefix() const;
    virtual void setLocalLabelsPrefix(SourceLocation* location, std::string prefix);

    virtual Expr* condition() const;

    ProgramSection* currentSection() const { return mSection; }
    virtual bool setCurrentSection(ProgramSection* section);

    virtual void addInstruction(Instruction* instruction);
    virtual void addConstant(SymbolTable* symbolTable, SourceLocation* location, const char* name, Expr* value);
    virtual void addLabel(SymbolTable* symbolTable, SourceLocation* location, const char* name);
    virtual Label* addEphemeralLabel(SourceLocation* location);
    int allocEphemeralLabelCounter();

private:
    AssemblerContext* mPrev;
    ProgramSection* mSection;
    std::string mLocalLabelsPrefix;
    int mEphemeralLabelCounter;

    DISABLE_COPY(AssemblerContext);
};

#endif
