#include "SymbolTable.h"

SymbolTable::SymbolTable(SymbolTable* parent)
    : mParent(parent)
{
    registerFinalizer();
}

SymbolTable::~SymbolTable()
{
}
