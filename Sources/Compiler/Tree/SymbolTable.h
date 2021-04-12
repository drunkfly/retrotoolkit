#ifndef COMPILER_TREE_SYMBOLTABLE_H
#define COMPILER_TREE_SYMBOLTABLE_H

#include "Common/GC.h"

class Symbol;

class SymbolTable : public GCObject
{
public:
    explicit SymbolTable(SymbolTable* parent, bool passthrough = false);
    ~SymbolTable() override;

    SymbolTable* parent() const { return mParent; }

    const std::unordered_map<std::string, Symbol*>& symbols() const { return mSymbols; }

    bool addSymbol(Symbol* symbol);
    bool addLocalSymbol(Symbol* symbol);
    Symbol* findSymbol(const std::string& name) const;

private:
    SymbolTable* mParent;
    std::unordered_map<std::string, Symbol*> mSymbols;
    bool mPassThrough;

    DISABLE_COPY(SymbolTable);
};

#endif
