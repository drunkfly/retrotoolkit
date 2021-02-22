#include "ParsingContext.h"
#include "Common/Strings.h"
#include "Compiler/ExpressionParser.h"
#include "Compiler/Token.h"
#include <ctype.h>

void ParsingContext::nextToken()
{
    mToken = mToken->next();
}

bool ParsingContext::expression(Expr*& expr,
    const StringSet* registerNames, const StringSet* conditionNames, bool unambiguous)
{
    ExpressionParser parser(mHeap, registerNames, conditionNames);
    expr = parser.tryParseExpression(this, unambiguous);
    return (expr != nullptr);
}

bool ParsingContext::expressionInParentheses(Expr*& expr,
    const StringSet* registerNames, const StringSet* conditionNames)
{
    if (!consumeLeftParenthesis())
        return false;
    if (!expression(expr, registerNames, conditionNames, true))
        return false;
    return consumeRightParenthesis();
}

bool ParsingContext::consumeComma()
{
    if (mToken->id() != TOK_COMMA)
        return false;
    nextToken();
    return true;
}

bool ParsingContext::consumeLeftParenthesis()
{
    if (mToken->id() != TOK_LPAREN)
        return false;
    nextToken();
    return true;
}

bool ParsingContext::consumeRightParenthesis()
{
    if (mToken->id() != TOK_RPAREN)
        return false;
    nextToken();
    return true;
}

bool ParsingContext::consumeIdentifier(const char* name)
{
    if (mToken->id() < TOK_IDENTIFIER)
        return false;
    if (!equalCaseInsensitive(mToken->text(), name))
        return false;
    nextToken();
    return true;
}

bool ParsingContext::consumeIdentifierInParentheses(const char* name)
{
    return consumeLeftParenthesis() && consumeIdentifier(name) && consumeRightParenthesis();
}

void ParsingContext::end()
{
    mTokenRef = mToken;
}

bool ParsingContext::checkEnd()
{
    if (mToken->id() != TOK_EOF && !mToken->isFirstOnLine())
        return false;
    end();
    return true;
}
