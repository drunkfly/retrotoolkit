#include "ParsingContext.h"
#include "Common/Strings.h"
#include "Compiler/CompilerError.h"
#include "Compiler/ExpressionParser.h"
#include "Compiler/Token.h"

void ParsingContext::nextToken()
{
    mToken = mToken->next();
}

bool ParsingContext::expression(Expr*& expr,
    const StringSet* registerNames, const StringSet* conditionNames, bool unambiguous)
{
    ExpressionParser parser(mHeap, registerNames, conditionNames, mLocalLabelsPrefix);
    ParsingContext subcontext(mHeap, mToken, mSymbolTable, mLocalLabelsPrefix, mAllowEol);
    expr = parser.tryParseExpression(&subcontext, unambiguous);
    return (expr != nullptr);
}

bool ParsingContext::expressionInParentheses(Expr*& expr,
    const StringSet* registerNames, const StringSet* conditionNames, bool unambiguous)
{
    if (!consumeLeftParenthesis())
        return false;
    if (!expression(expr, registerNames, conditionNames, unambiguous))
        return false;
    return consumeRightParenthesis();
}

Expr* ParsingContext::unambiguousExpression()
{
    ExpressionParser parser(mHeap, nullptr, nullptr, mLocalLabelsPrefix);
    ParsingContext subcontext(mHeap, mToken, mSymbolTable, mLocalLabelsPrefix, mAllowEol);
    Expr* expr = parser.tryParseExpression(&subcontext, true);
    if (!expr)
        throw CompilerError(parser.errorLocation(), parser.error());
    end();
    return expr;
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

bool ParsingContext::isAtEol() const
{
    return (!mAllowEol && mToken->isFirstOnLine());
}

void ParsingContext::ensureNotEol()
{
    if (!mAllowEol && mToken->isFirstOnLine()) {
        auto loc = mToken->location();
        auto file = (loc ? loc->file() : nullptr);
        int line = (loc && loc->line() > 1 ? loc->line() - 1 : 0);
        throw CompilerError(new (mHeap) SourceLocation(file, line), "unexpected end of line.");
    }
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
