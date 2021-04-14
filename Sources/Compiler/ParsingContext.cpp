#include "ParsingContext.h"
#include "Common/Strings.h"
#include "Compiler/Assembler/AssemblerContext.h"
#include "Compiler/CompilerError.h"
#include "Compiler/ExpressionParser.h"
#include "Compiler/Tree/Expr.h"
#include "Compiler/Token.h"

void ParsingContext::nextToken()
{
    mToken = mToken->next();
}

bool ParsingContext::expression(Expr*& expr,
    const StringSet* registerNames, const StringSet* conditionNames, bool unambiguous, bool allowHereVariable)
{
    ExpressionParser parser(mHeap, registerNames, conditionNames, mLocalLabelsPrefix);
    ParsingContext subcontext(mHeap, mToken, mContext, mSymbolTable, mLocalLabelsPrefix, mAllowEol);
    expr = parser.tryParseExpression(&subcontext, unambiguous, allowHereVariable);
    return (expr != nullptr);
}

bool ParsingContext::expressionInParentheses(Expr*& expr,
    const StringSet* registerNames, const StringSet* conditionNames, bool unambiguous, bool allowHereVariable)
{
    if (!consumeLeftParenthesis())
        return false;
    if (!expression(expr, registerNames, conditionNames, unambiguous, allowHereVariable))
        return false;
    return consumeRightParenthesis();
}

Expr* ParsingContext::unambiguousExpression(bool allowHereVariable)
{
    ExpressionParser parser(mHeap, nullptr, nullptr, mLocalLabelsPrefix);
    ParsingContext subcontext(mHeap, mToken, mContext, mSymbolTable, mLocalLabelsPrefix, mAllowEol);
    Expr* expr = parser.tryParseExpression(&subcontext, true, allowHereVariable);
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

void ParsingContext::setupHereVariable(Expr* expr, uint64_t offset)
{
    if (expr->isHereVariable()) {
        auto var = static_cast<ExprVariableHere*>(expr);

        if (!mContext)
            throw CompilerError(expr->location(), "@here variable is not allowed in this context.");

        if (var->variableName()->id() >= TOK_IDENTIFIER)
            mContext->addLabel(mSymbolTable, var->variableName()->location(), var->variableName()->text(), offset);
        else if (var->variableName()->id() == TOK_LABEL_LOCAL_NAME) {
            const auto& prefix = mContext->localLabelsPrefix();
            if (prefix.empty())
                throw CompilerError(var->variableName()->location(), "local label name without preceding global label.");

            std::stringstream ss;
            ss << prefix;
            ss << "@@";
            ss << var->variableName()->text();
            std::string str = ss.str();
            auto name = mHeap->allocString(str.data(), str.length());

            mContext->addLabel(mSymbolTable, var->variableName()->location(), name, offset);
        } else
            throw CompilerError(var->variableName()->location(), "internal compiler error: invalid name token.");

        return;
    }

    if (expr->containsHereVariable())
        throw CompilerError(expr->location(), "@here variable cannot be used in expression.");
}

void ParsingContext::rejectHereVariableInIXIY(Expr* expr)
{
    if (expr->isHereVariable())
        throw CompilerError(expr->location(), "@here should not be used with IX-# or IY-#. Use IX+# or IY+# instead.");

    if (expr->containsHereVariable())
        throw CompilerError(expr->location(), "@here variable cannot be used in expression.");
}
