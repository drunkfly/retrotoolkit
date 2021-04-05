#ifndef COMPILER_ASSEMBLER_PARSINGCONTEXT_H
#define COMPILER_ASSEMBLER_PARSINGCONTEXT_H

#include "Common/StringSet.h"

class GCHeap;
class Token;
class SymbolTable;
class Expr;

class ParsingContext
{
public:
    ParsingContext(GCHeap* heap, const Token*& token,
            SymbolTable* symbolTable, const std::string* localLabelsPrefix, bool allowEol)
        : mHeap(heap)
        , mToken(token)
        , mTokenRef(token)
        , mLocalLabelsPrefix(localLabelsPrefix)
        , mSymbolTable(symbolTable)
        , mAllowEol(allowEol)
    {
    }

    GCHeap* heap() const { return mHeap; }
    SymbolTable* symbolTable() const { return mSymbolTable; }

    const Token* token() const { return mToken; }
    void nextToken();

    bool expression(Expr*& expr, const StringSet* registerNames, const StringSet* conditionNames, bool unambiguous);
    bool expressionInParentheses(Expr*& expr,
        const StringSet* registerNames, const StringSet* conditionNames, bool unambiguous);
    Expr* unambiguousExpression();

    bool consumeComma();
    bool consumeLeftParenthesis();
    bool consumeRightParenthesis();
    bool consumeIdentifier(const char* name);
    bool consumeIdentifierInParentheses(const char* name);

    bool isAtEol() const;
    void ensureNotEol();

    void end();
    bool checkEnd();

private:
    GCHeap* mHeap;
    const Token* mToken;
    const Token*& mTokenRef;
    const std::string* mLocalLabelsPrefix;
    SymbolTable* mSymbolTable;
    bool mAllowEol;

    DISABLE_COPY(ParsingContext);
};

#endif
