#ifndef COMPILER_LINKER_PROGRAM_H
#define COMPILER_LINKER_PROGRAM_H

#include "Common/GC.h"
#include <unordered_map>

class SymbolTable;
class ProgramSection;

class Program : public GCObject
{
public:
    Program();
    ~Program();

    SymbolTable* globals() const { return mGlobals; }

    ProgramSection* getSection(const std::string& name) const;
    ProgramSection* getOrAddSection(const std::string& name);

private:
    SymbolTable* mGlobals;
    std::unordered_map<std::string, ProgramSection*> mSections;

    DISABLE_COPY(Program);
};

#endif
