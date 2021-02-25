#include "SymbolTable.h"
#include "Compiler/Tree/Symbol.h"

SymbolTable::SymbolTable(SymbolTable* parent)
    : mParent(parent)
{
    registerFinalizer();
}

SymbolTable::~SymbolTable()
{
}

bool SymbolTable::addSymbol(Symbol* symbol)
{
    return mSymbols.emplace(symbol->name(), symbol).second;
}

Symbol* SymbolTable::findSymbol(const std::string& name) const
{
    const SymbolTable* table = this;
    do {
        auto it = table->mSymbols.find(name);
        if (it != table->mSymbols.end())
            return it->second;
        table = table->mParent;
    } while (table);
    return nullptr;
}
