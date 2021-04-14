#ifndef COMPILER_EXPRESSIONPARSER_H
#define COMPILER_EXPRESSIONPARSER_H

#include "Common/StringSet.h"

class GCHeap;
class ParsingContext;
class SymbolTable;
class SourceLocation;
class Expr;

class ExpressionParser
{
public:
    ExpressionParser(GCHeap* heap, const StringSet* registerNames,
        const StringSet* conditionNames, const std::string* localLabelsPrefix);
    ~ExpressionParser();

    const std::string& error() const { return mError; }
    SourceLocation* errorLocation() const { return mErrorLocation; }

    Expr* tryParseExpression(SourceLocation* location, const char* str, SymbolTable* variables = nullptr);
    Expr* tryParseExpression(ParsingContext* context, bool unambiguous, bool allowHereVariable);

private:
    static const std::unordered_map<std::string, Expr*(ExpressionParser::*)()> mBuiltInFunctions;

    GCHeap* mHeap;
    ParsingContext* mContext;
    const StringSet* mRegisterNames;
    const StringSet* mConditionNames;
    const std::string* mLocalLabelsPrefix;
    std::string mError;
    SourceLocation* mErrorLocation;

    Expr* parseExpression(bool unambiguous, bool allowHereVariable);
    Expr* parseConditionalExpression(bool unambiguous, bool allowHereVariable);
    Expr* parseLogicOrExpression(bool unambiguous, bool allowHereVariable);
    Expr* parseLogicAndExpression(bool unambiguous, bool allowHereVariable);
    Expr* parseBitwiseOrExpression(bool unambiguous, bool allowHereVariable);
    Expr* parseBitwiseXorExpression(bool unambiguous, bool allowHereVariable);
    Expr* parseBitwiseAndExpression(bool unambiguous, bool allowHereVariable);
    Expr* parseEqualityExpression(bool unambiguous, bool allowHereVariable);
    Expr* parseRelationalExpression(bool unambiguous, bool allowHereVariable);
    Expr* parseShiftExpression(bool unambiguous, bool allowHereVariable);
    Expr* parseAdditionExpression(bool unambiguous, bool allowHereVariable);
    Expr* parseMultiplicationExpression(bool unambiguous, bool allowHereVariable);
    Expr* parseUnaryExpression(bool unambiguous, bool allowHereVariable);
    Expr* parseAtomicExpression(bool unambiguous, bool allowHereVariable);

    Expr* parseAddressOfFunction();
    Expr* parseBaseOfFunction();
    Expr* parseSizeOfFunction();

    DISABLE_COPY(ExpressionParser);
};

#endif
