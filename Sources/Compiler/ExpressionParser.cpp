#include "ExpressionParser.h"
#include "Common/Strings.h"
#include "Compiler/Tree/Expr.h"
#include "Compiler/ParsingContext.h"
#include "Compiler/ExpressionParser.h"
#include "Compiler/Token.h"
#include "Compiler/Lexer.h"

ExpressionParser::ExpressionParser(GCHeap* heap,
        const StringSet* registerNames, const StringSet* conditionNames, const std::string* localLabelsPrefix)
    : mHeap(heap)
    , mRegisterNames(registerNames)
    , mConditionNames(conditionNames)
    , mLocalLabelsPrefix(localLabelsPrefix)
    , mErrorLocation(nullptr)
{
}

ExpressionParser::~ExpressionParser()
{
}

Expr* ExpressionParser::tryParseExpression(SourceLocation* location, const char* str, SymbolTable* variables)
{
    Lexer lexer(mHeap, Lexer::Mode::SingleLineExpression);
    lexer.scan((location ? location->file() : nullptr), str, (location ? location->line() : 1));

    if (!variables)
        variables = new (mHeap) SymbolTable(nullptr);

    const Token* token = lexer.firstToken();
    ParsingContext context(mHeap, token, variables, mLocalLabelsPrefix, false);
    return tryParseExpression(&context, true);
}

Expr* ExpressionParser::tryParseExpression(ParsingContext* context, bool unambiguous)
{
    mContext = context;
    mError.clear();
    mErrorLocation = nullptr;

    auto expr = parseExpression(unambiguous);
    if (!expr)
        return nullptr;

    context->end();
    return expr;
}

Expr* ExpressionParser::parseExpression(bool unambiguous)
{
    return parseConditionalExpression(unambiguous);
}

Expr* ExpressionParser::parseConditionalExpression(bool unambiguous)
{
    auto expr = parseLogicOrExpression(unambiguous);
    if (!expr)
        return nullptr;

    if (mContext->token()->id() == TOK_QUESTION) {
        mContext->ensureNotEol();
        auto location = mContext->token()->location();

        mContext->nextToken();
        auto opThen = parseExpression(true);
        if (!opThen)
            return nullptr;

        if (mContext->token()->id() != TOK_COLON) {
            mError = "expected ':'.";
            mErrorLocation = location;
            return nullptr;
        }

        mContext->ensureNotEol();
        mContext->nextToken();
        auto opElse = parseConditionalExpression(true);
        if (!opElse)
            return nullptr;

        expr = new (mHeap) ExprConditional(location, expr, opThen, opElse);
    }

    return expr;
}

#define BINARY_OPERATOR_METHOD(NAME, TOKEN, OPERAND) \
    Expr* ExpressionParser::parse##NAME##Expression(bool unambiguous) \
    { \
        auto expr = parse##OPERAND##Expression(unambiguous); \
        if (!expr) \
            return nullptr; \
        \
        while (mContext->token()->id() == TOKEN) { \
            mContext->ensureNotEol(); \
            \
            auto location = mContext->token()->location(); \
            mContext->nextToken(); \
            \
            auto op2 = parse##OPERAND##Expression(true); \
            if (!op2) \
                return nullptr; \
            \
            expr = new (mHeap) Expr##NAME(location, expr, op2); \
        } \
        \
        return expr; \
    }

BINARY_OPERATOR_METHOD(LogicOr, TOK_DOUBLEVBAR, LogicAnd)
BINARY_OPERATOR_METHOD(LogicAnd, TOK_DOUBLEAMPERSAND, BitwiseOr)
BINARY_OPERATOR_METHOD(BitwiseOr, TOK_VBAR, BitwiseXor)
BINARY_OPERATOR_METHOD(BitwiseXor, TOK_CARET, BitwiseAnd)
BINARY_OPERATOR_METHOD(BitwiseAnd, TOK_AMPERSAND, Equality)

Expr* ExpressionParser::parseEqualityExpression(bool unambiguous)
{
    auto expr = parseRelationalExpression(unambiguous);
    if (!expr)
        return nullptr;

    while (mContext->token()->id() == TOK_EQ || mContext->token()->id() == TOK_INEQ) {
        mContext->ensureNotEol();

        const Token* token = mContext->token();
        mContext->nextToken();

        auto op2 = parseRelationalExpression(true);
        if (!op2)
            return nullptr;

        switch (token->id()) {
            case TOK_EQ: expr = new (mHeap) ExprEqual(token->location(), expr, op2); break;
            case TOK_INEQ: expr = new (mHeap) ExprNotEqual(token->location(), expr, op2); break;
        }
    }

    return expr;
}

Expr* ExpressionParser::parseRelationalExpression(bool unambiguous)
{
    auto expr = parseShiftExpression(unambiguous);
    if (!expr)
        return nullptr;

    while (mContext->token()->id() == TOK_LESS || mContext->token()->id() == TOK_LESSEQ
            || mContext->token()->id() == TOK_GREATER || mContext->token()->id() == TOK_GREATEREQ) {
        mContext->ensureNotEol();

        const Token* token = mContext->token();
        mContext->nextToken();

        auto op2 = parseShiftExpression(true);
        if (!op2)
            return nullptr;

        switch (token->id()) {
            case TOK_LESS: expr = new (mHeap) ExprLess(token->location(), expr, op2); break;
            case TOK_LESSEQ: expr = new (mHeap) ExprLessEqual(token->location(), expr, op2); break;
            case TOK_GREATER: expr = new (mHeap) ExprGreater(token->location(), expr, op2); break;
            case TOK_GREATEREQ: expr = new (mHeap) ExprGreaterEqual(token->location(), expr, op2); break;
        }
    }

    return expr;
}

Expr* ExpressionParser::parseShiftExpression(bool unambiguous)
{
    auto expr = parseAdditionExpression(unambiguous);
    if (!expr)
        return nullptr;

    while (mContext->token()->id() == TOK_SHL || mContext->token()->id() == TOK_SHR) {
        mContext->ensureNotEol();

        const Token* token = mContext->token();
        mContext->nextToken();

        auto op2 = parseAdditionExpression(true);
        if (!op2)
            return nullptr;

        switch (token->id()) {
            case TOK_SHL: expr = new (mHeap) ExprShiftLeft(token->location(), expr,op2); break;
            case TOK_SHR: expr = new (mHeap) ExprShiftRight(token->location(), expr, op2); break;
        }
    }

    return expr;
}

Expr* ExpressionParser::parseAdditionExpression(bool unambiguous)
{
    auto expr = parseMultiplicationExpression(unambiguous);
    if (!expr)
        return nullptr;

    while (mContext->token()->id() == TOK_PLUS || mContext->token()->id() == TOK_MINUS) {
        mContext->ensureNotEol();

        const Token* token = mContext->token();
        mContext->nextToken();

        auto op2 = parseMultiplicationExpression(true);
        if (!op2)
            return nullptr;

        switch (token->id()) {
            case TOK_PLUS: expr = new (mHeap) ExprAdd(token->location(), expr, op2); break;
            case TOK_MINUS: expr = new (mHeap) ExprSubtract(token->location(), expr, op2); break;
        }
    }

    return expr;
}

Expr* ExpressionParser::parseMultiplicationExpression(bool unambiguous)
{
    auto expr = parseUnaryExpression(unambiguous);
    if (!expr)
        return nullptr;

    while (mContext->token()->id() == TOK_ASTERISK || mContext->token()->id() == TOK_SLASH
            || mContext->token()->id() == TOK_PERCENT) {
        mContext->ensureNotEol();

        const Token* token = mContext->token();
        mContext->nextToken();

        auto op2 = parseUnaryExpression(true);
        if (!op2)
            return nullptr;

        switch (token->id()) {
            case TOK_ASTERISK: expr = new (mHeap) ExprMultiply(token->location(), expr, op2); break;
            case TOK_SLASH: expr = new (mHeap) ExprDivide(token->location(), expr, op2); break;
            case TOK_PERCENT: expr = new (mHeap) ExprModulo(token->location(), expr, op2); break;
        }
    }

    return expr;
}

Expr* ExpressionParser::parseUnaryExpression(bool unambiguous)
{
    switch (mContext->token()->id()) {
        case TOK_MINUS: {
            mContext->ensureNotEol();
            auto location = mContext->token()->location();
            mContext->nextToken();
            auto operand = parseAtomicExpression(true);
            if (!operand)
                return nullptr;
            return new (mHeap) ExprNegate(location, operand);
        }

        case TOK_PLUS:
            mContext->ensureNotEol();
            mContext->nextToken();
            return parseAtomicExpression(true);

        case TOK_EXCLAMATION: {
            mContext->ensureNotEol();
            auto location = mContext->token()->location();
            mContext->nextToken();
            auto operand = parseAtomicExpression(true);
            if (!operand)
                return nullptr;
            return new (mHeap) ExprLogicNot(location, operand);
        }

        case TOK_TILDE: {
            mContext->ensureNotEol();
            auto location = mContext->token()->location();
            mContext->nextToken();
            auto operand = parseAtomicExpression(true);
            if (!operand)
                return nullptr;
            return new (mHeap) ExprBitwiseNot(location, operand);
        }
    }

    return parseAtomicExpression(unambiguous);
}

Expr* ExpressionParser::parseAtomicExpression(bool unambiguous)
{
    switch (mContext->token()->id()) {
        case TOK_CHAR:
        case TOK_NUMBER: {
            mContext->ensureNotEol();
            Expr* expr = new (mHeap) ExprNumber(mContext->token()->location(), mContext->token()->number());
            mContext->nextToken();
            return expr;
        }

        case TOK_DOLLAR: {
            mContext->ensureNotEol();
            Expr* expr = new (mHeap) ExprCurrentAddress(mContext->token()->location());
            mContext->nextToken();
            return expr;
        }

        case TOK_LABEL_LOCAL_NAME: {
            mContext->ensureNotEol();

            if (!mLocalLabelsPrefix || mLocalLabelsPrefix->empty()) {
                mErrorLocation = mContext->token()->location();
                mError = "local label name without preceding global label.";
                return nullptr;
            }

            std::stringstream ss;
            ss << *mLocalLabelsPrefix;
            ss << "@@";
            ss << mContext->token()->text();
            std::string name = ss.str();

            Expr* expr = new (mHeap) ExprIdentifier(mContext->token()->location(),
                mContext->symbolTable(), mHeap->allocString(name.c_str(), name.length()));
            mContext->nextToken();

            return expr;
        }

        case TOK_IDENTIFIER: {
            mContext->ensureNotEol();

            if (!unambiguous && (mRegisterNames || mConditionNames)) {
                std::string lowerText = toLower(mContext->token()->text());
                if (mRegisterNames && mRegisterNames->find(lowerText) != mRegisterNames->end()) {
                    std::stringstream ss;
                    ss << "'" << mContext->token()->text()
                        << "' is ambiguous in this context (could be expression or register).";
                    mError = ss.str();
                    mErrorLocation = mContext->token()->location();
                    return nullptr;
                }
                if (mConditionNames && mConditionNames->find(lowerText) != mConditionNames->end()) {
                    std::stringstream ss;
                    ss << "'" << mContext->token()->text()
                        << "' is ambiguous in this context (could be expression or jump condition name).";
                    mError = ss.str();
                    mErrorLocation = mContext->token()->location();
                    return nullptr;
                }
            }

            Expr* expr = new (mHeap) ExprIdentifier(mContext->token()->location(),
                mContext->symbolTable(), mContext->token()->text());
            mContext->nextToken();

            return expr;
        }

        case TOK_LPAREN:
            mContext->ensureNotEol();

            if (unambiguous) {
                mContext->nextToken();
                auto expr = parseExpression(true);
                if (!expr)
                    return nullptr;

                if (mContext->token()->id() != TOK_RPAREN) {
                    mError = "expected ')'.";
                    mErrorLocation = mContext->token()->location();
                    return nullptr;
                }

                mContext->ensureNotEol();
                mContext->nextToken();
                return expr;
            }

            mError = "'(' is ambiguous in this context (could be expression or memory reference).";
            mErrorLocation = mContext->token()->location();
            return nullptr;

        default:
            mError = "expected expression.";
            mErrorLocation = mContext->token()->location();
            return nullptr;
    }
}
