#ifndef COMPILER_TREE_SYMBOLTABLE_H
#define COMPILER_TREE_SYMBOLTABLE_H

#include "Common/GC.h"
#include <string>
#include <unordered_map>

class Symbol;

class SymbolTable : public GCObject
{
public:
    explicit SymbolTable(SymbolTable* parent);
    ~SymbolTable() override;

    SymbolTable* parent() const { return mParent; }

    bool addSymbol(Symbol* symbol);

private:
    SymbolTable* mParent;
    std::unordered_map<std::string, Symbol*> mSymbols;

    DISABLE_COPY(SymbolTable);
};

#endif
