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

    const std::unordered_map<std::string, Symbol*>& symbols() const { return mSymbols; }

    bool addSymbol(Symbol* symbol);
    Symbol* findSymbol(const std::string& name) const;

private:
    SymbolTable* mParent;
    std::unordered_map<std::string, Symbol*> mSymbols;

    DISABLE_COPY(SymbolTable);
};

#endif
