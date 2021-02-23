#ifndef COMPILER_EXPRESSIONPARSER_H
#define COMPILER_EXPRESSIONPARSER_H

#include "Common/Common.h"
#include "Common/StringSet.h"
#include <string>

class GCHeap;
class ParsingContext;
class SymbolTable;
class SourceLocation;
class Expr;

class ExpressionParser
{
public:
    ExpressionParser(GCHeap* heap, const StringSet* registerNames, const StringSet* conditionNames);
    ~ExpressionParser();

    const std::string& error() const { return mError; }
    SourceLocation* errorLocation() const { return mErrorLocation; }

    Expr* tryParseExpression(const char* str, SymbolTable* variables = nullptr);
    Expr* tryParseExpression(ParsingContext* context, bool unambiguous);

private:
    GCHeap* mHeap;
    ParsingContext* mContext;
    const StringSet* mRegisterNames;
    const StringSet* mConditionNames;
    std::string mError;
    SourceLocation* mErrorLocation;

    Expr* parseExpression(bool unambiguous);
    Expr* parseConditionalExpression(bool unambiguous);
    Expr* parseLogicOrExpression(bool unambiguous);
    Expr* parseLogicAndExpression(bool unambiguous);
    Expr* parseBitwiseOrExpression(bool unambiguous);
    Expr* parseBitwiseXorExpression(bool unambiguous);
    Expr* parseBitwiseAndExpression(bool unambiguous);
    Expr* parseEqualityExpression(bool unambiguous);
    Expr* parseRelationalExpression(bool unambiguous);
    Expr* parseShiftExpression(bool unambiguous);
    Expr* parseAdditionExpression(bool unambiguous);
    Expr* parseMultiplicationExpression(bool unambiguous);
    Expr* parseUnaryExpression(bool unambiguous);
    Expr* parseAtomicExpression(bool unambiguous);

    DISABLE_COPY(ExpressionParser);
};

#endif
