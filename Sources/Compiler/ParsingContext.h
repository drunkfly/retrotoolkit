#ifndef COMPILER_ASSEMBLER_PARSINGCONTEXT_H
#define COMPILER_ASSEMBLER_PARSINGCONTEXT_H

#include "Common/StringSet.h"

class GCHeap;
class Token;
class SymbolTable;
class Expr;
class AssemblerContext;

class ParsingContext
{
public:
    ParsingContext(GCHeap* heap, const Token*& token, AssemblerContext* context,
            SymbolTable* symbolTable, const std::string* localLabelsPrefix, bool allowEol)
        : mHeap(heap)
        , mToken(token)
        , mTokenRef(token)
        , mLocalLabelsPrefix(localLabelsPrefix)
        , mContext(context)
        , mSymbolTable(symbolTable)
        , mAllowEol(allowEol)
    {
    }

    GCHeap* heap() const { return mHeap; }
    SymbolTable* symbolTable() const { return mSymbolTable; }

    const Token* token() const { return mToken; }
    void nextToken();

    bool expression(Expr*& expr,
        const StringSet* registerNames, const StringSet* conditionNames, bool unambiguous, bool allowHereVariable);
    bool expressionInParentheses(Expr*& expr,
        const StringSet* registerNames, const StringSet* conditionNames, bool unambiguous, bool allowHereVariable);
    Expr* unambiguousExpression(bool allowHereVariable);

    bool consumeComma();
    bool consumeLeftParenthesis();
    bool consumeRightParenthesis();
    bool consumeIdentifier(const char* name);
    bool consumeIdentifierInParentheses(const char* name);

    bool isAtEol() const;
    void ensureNotEol();

    void end();
    bool checkEnd();

    void setupHereVariable(Expr* expr, uint64_t offset);
    void rejectHereVariableInIXIY(Expr* expr);

private:
    GCHeap* mHeap;
    const Token* mToken;
    const Token*& mTokenRef;
    const std::string* mLocalLabelsPrefix;
    SymbolTable* mSymbolTable;
    AssemblerContext* mContext;
    bool mAllowEol;

    DISABLE_COPY(ParsingContext);
};

#endif
