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
