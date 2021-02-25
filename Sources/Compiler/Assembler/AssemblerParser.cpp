#include "AssemblerParser.h"
#include "Compiler/Token.h"
#include "Compiler/ParsingContext.h"
#include "Compiler/CompilerError.h"
#include "Compiler/Assembler/Instructions.Z80.h"
#include "Compiler/Assembler/AssemblerContext.h"
#include "Compiler/Assembler/Label.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/Linker/Program.h"
#include "Compiler/Linker/ProgramSection.h"
#include "Common/GC.h"
#include "Common/Strings.h"
#include <string>
#include <sstream>

/*
std::unordered_map<std::string, void(AssemblerParser::*)()> AssemblerParser::mDataDirectives = {
        { "db", &AssemblerParser::parseDefByte },
        { "dw", &AssemblerParser::parseDefWord },
        { "dd", &AssemblerParser::parseDefDWord },
        { "defb", &AssemblerParser::parseDefByte },
        { "defm", &AssemblerParser::parseDefByte },
        { "defw", &AssemblerParser::parseDefWord },
        { "defs", &AssemblerParser::parseDefSpace },
    };
*/

std::unordered_map<std::string, void(AssemblerParser::*)()> AssemblerParser::mDirectives = {
        { "section", &AssemblerParser::parseSectionDecl },
        /*
        { "repeat", &AssemblerParser::parseRepeatDecl },
        { "endrepeat", &AssemblerParser::parseEndRepeatDecl },
        { "if", &AssemblerParser::parseIfDecl },
        { "else", &AssemblerParser::parseElseDecl },
        { "endif", &AssemblerParser::parseEndIfDecl },
        { "allowwrite", &AssemblerParser::parseAllowWrite },
        { "disallowwrite", &AssemblerParser::parseDisallowWrite },
        { "pushallowwrite", &AssemblerParser::parsePushAllowWrite },
        { "pushdisallowwrite", &AssemblerParser::parsePushDisallowWrite },
        { "popallowwrite", &AssemblerParser::parsePopAllowWrite },
        { "popallowwriteafter", &AssemblerParser::parsePopAllowWriteAfter },
        { "assertbank", &AssemblerParser::parseAssertBank },
        */
    };

AssemblerParser::AssemblerParser(GCHeap* heap, Program* program)
    : mHeap(heap)
    , mContext(nullptr)
    , mProgram(program)
    , mSymbolTable(program->globals())
    , mToken(nullptr)
{
}

AssemblerParser::~AssemblerParser()
{
}

void AssemblerParser::parse(const Token* tokens)
{
    mContext = new (mHeap) AssemblerContext(nullptr);
    mSymbolTable = mProgram->globals();

    mToken = tokens;
    while (mToken->id() != TOK_EOF)
        parseLine();

    /*
    if (mContext->prev()) {
        if (mContext->isRepeat())
            error(tr("missing 'endrepeat'"));
        else if (mContext->isIf())
            error(tr("missing 'endif'"));
        else {
            Q_ASSERT(false);
            error(tr("internal compiler error"));
        }
    }
    */
}

/*
template <typename T, typename... ARGS> T* AssemblerParser::pushContext(ARGS&&... args)
{
    auto uniq = std::make_unique<T>(mCompiler, std::move(mContext), std::forward<ARGS>(args)...);
    auto ptr = uniq.get();
    mContext = std::move(uniq);
    return ptr;
}

void AssemblerParser::popContext()
{
    mContext = mContext->takePrev();
    Q_ASSERT(mContext != nullptr);
    if (!mContext)
        error(tr("internal compiler error"));
}
*/

void AssemblerParser::parseLine()
{
    if (mToken->id() == TOK_HASH) {
        mToken = mToken->next();

        if (mToken->id() >= TOK_IDENTIFIER) {
            auto str = toLower(mToken->text());
            auto it = mDirectives.find(str);
            if (it != mDirectives.end()) {
                (this->*(it->second))();
                return;
            }
        }

        std::stringstream ss;
        ss << "unexpected " << mToken->name() << '.';
        throw CompilerError(mToken->location(), ss.str());
    }

    Label* label = nullptr;

    // read label, if any
    if (mToken->id() == TOK_LABEL_GLOBAL || mToken->id() == TOK_LABEL_FULL || mToken->id() == TOK_LABEL_LOCAL) {
        /*
        if (mToken->id != TOK_LABEL_LOCAL && !mContext->areGlobalLabelsAllowed())
            error(tr("global labels are not allowed in this context"));
        */

        std::string name = readLabelName();
        mContext->addLabel(mSymbolTable, mToken->location(), std::move(name));
        mToken = mToken->next();

        // skip label-only lines
        if (matchEol())
            return;
    }

    // read directive / instruction
    if (mToken->id() >= TOK_IDENTIFIER) {
        Instruction* instruction = parseOpcode();
        if (instruction) {
            mContext->addInstruction(instruction);
            return;
        }

        /*
        auto jt = mDataDirectives.find(str);
        if (jt != mDataDirectives.end()) {
            (this->*(jt->second))();
            return;
        }
        */
    }

    // 'equ' and data directives handling (may be preceded by label without ':')

    // read label name
    const Token* nameToken = mToken;
    std::string name;
    if (mToken->id() >= TOK_IDENTIFIER)
        name = nameToken->text();
    else if (mToken->id() == TOK_LABEL_LOCAL_NAME)
        name = readLabelName();
    else
        throw CompilerError(nameToken->location(), "expected opcode or directive");

    // read directive
    mToken = mToken->next();
    /*
    std::unordered_map<std::string, void(AssemblerParser::*)()>::iterator iter;
    std::string lower = toLower(mToken->text());
    if (lastTokenId() >= TOK_IDENTIFIER && (iter = mDataDirectives.find(lower)) != mDataDirectives.end()) {
        if (nameToken.id != T_LOCAL_LABEL_NAME && !mContext->areGlobalLabelsAllowed())
            error(tr("global labels are not allowed in this context"));

        label = mProgram->addLabel(nameToken, mContext->codeEmitter(), name);
        if (!label || mContext->hasVariable(name))
            error(tr("duplicate identifier '%1'").arg(name.c_str()));

        mContext->adjustLabel(label);

        if (nameToken.id >= TOK_IDENTIFIER)
            mContext->setLocalLabelsPrefix(name, nameToken, mReporter);
        (this->*(iter->second))();
    } else if ((lastTokenId() >= TOK_IDENTIFIER && lower == "equ") || lastTokenId() == T_ASSIGN) {
        auto expr = parseExpression(nextToken(), true);
        if (!expr)
            error(tr("expected expression after 'equ'"));
        if (!mProgram->addConstant(name, std::move(expr)) || mContext->hasVariable(name))
            error(nameToken, tr("duplicate identifier '%1'").arg(name.c_str()));
    } else */ {
        std::stringstream ss;
        ss << "unexpected " << nameToken->name() << '.';
        throw CompilerError(nameToken->location(), ss.str());
    }
}

void AssemblerParser::parseSectionDecl()
{
    mToken = mToken->next();

    SourceLocation* location = mToken->location();
    const char* sectionName = consumeIdentifier();
    auto section = mProgram->getOrAddSection(sectionName);

    if (!mContext->setCurrentSection(section))
        throw CompilerError(location, "'section' directive is not allowed in this context.");

    expectEol();
}

/*
void AssemblerParser::parseRepeatDecl()
{
    Token token = lastToken();

    auto count = parseExpression(nextToken(), true);
    std::string variable;

    if (lastTokenId() == T_COMMA) {
        variable = expectIdentifier(nextToken());
        if (mContext->hasVariable(variable) || mProgram->isDeclared(variable))
            error(tr("duplicate identifier '%1'").arg(variable.c_str()));
        nextToken();
    }

    auto parentCodeEmitter = mContext->codeEmitter();

    auto context = pushContext<AssemblerRepeatContext>(token, std::move(variable), std::move(count));
    parentCodeEmitter->emit<RepeatMacro>(token, context->codeEmitterSharedPtr());

    expectEol(lastTokenId());
}

void AssemblerParser::parseEndRepeatDecl()
{
    if (!mContext->isRepeat())
        error(tr("mismatched 'endrepeat'"));

    popContext();

    expectEol(nextToken());
}

void AssemblerParser::parseIfDecl()
{
    Token token = lastToken();

    auto cond = parseExpression(nextToken(), true);
    auto parentCodeEmitter = mContext->codeEmitter();

    auto context = pushContext<AssemblerIfContext>(token);
    parentCodeEmitter->emit<IfMacro>(token, std::move(cond), context->thenCodeEmitter(), context->elseCodeEmitter());

    expectEol(lastTokenId());
}

void AssemblerParser::parseElseDecl()
{
    if (!mContext->isIf() || mContext->hasElse())
        error(tr("unexpected 'else'"));

    mContext->beginElse(mReporter, lastToken());

    expectEol(nextToken());
}

void AssemblerParser::parseEndIfDecl()
{
    if (!mContext->isIf())
        error(tr("mismatched 'endif'"));

    popContext();

    expectEol(nextToken());
}

void AssemblerParser::parseAllowWrite()
{
    Token token = lastToken();

    auto startExpr = parseExpression(nextToken(), true);
    if (!startExpr)
        error(mExpressionError);

    expectComma(lastTokenId());

    auto sizeExpr = parseExpression(nextToken(), true);
    if (!sizeExpr)
        error(mExpressionError);

    expectEol(lastTokenId());

    mContext->codeEmitter()->emit<WriteDirective>(token, std::move(startExpr), std::move(sizeExpr), ProgramWriteProtection::What::AllowWrite);
}

void AssemblerParser::parseDisallowWrite()
{
    Token token = lastToken();

    auto startExpr = parseExpression(nextToken(), true);
    if (!startExpr)
        error(mExpressionError);

    expectComma(lastTokenId());

    auto sizeExpr = parseExpression(nextToken(), true);
    if (!sizeExpr)
        error(mExpressionError);

    expectEol(lastTokenId());

    mContext->codeEmitter()->emit<WriteDirective>(token, std::move(startExpr), std::move(sizeExpr), ProgramWriteProtection::What::DisallowWrite);
}

void AssemblerParser::parsePushAllowWrite()
{
    Token token = lastToken();

    auto startExpr = parseExpression(nextToken(), true);
    if (!startExpr)
        error(mExpressionError);

    expectComma(lastTokenId());

    auto sizeExpr = parseExpression(nextToken(), true);
    if (!sizeExpr)
        error(mExpressionError);

    expectEol(lastTokenId());

    mContext->codeEmitter()->emit<WriteDirective>(token, std::move(startExpr), std::move(sizeExpr), ProgramWriteProtection::What::PushAllowWrite);
}

void AssemblerParser::parsePushDisallowWrite()
{
    Token token = lastToken();

    auto startExpr = parseExpression(nextToken(), true);
    if (!startExpr)
        error(mExpressionError);

    expectComma(lastTokenId());

    auto sizeExpr = parseExpression(nextToken(), true);
    if (!sizeExpr)
        error(mExpressionError);

    expectEol(lastTokenId());

    mContext->codeEmitter()->emit<WriteDirective>(token, std::move(startExpr), std::move(sizeExpr), ProgramWriteProtection::What::PushDisallowWrite);
}

void AssemblerParser::parsePopAllowWrite()
{
    Token token = lastToken();

    auto startExpr = parseExpression(nextToken(), true);
    if (!startExpr)
        error(mExpressionError);

    expectComma(lastTokenId());

    auto sizeExpr = parseExpression(nextToken(), true);
    if (!sizeExpr)
        error(mExpressionError);

    expectEol(lastTokenId());

    mContext->codeEmitter()->emit<WriteDirective>(token, std::move(startExpr), std::move(sizeExpr), ProgramWriteProtection::What::PopAllowWriteEarly);
}

void AssemblerParser::parsePopAllowWriteAfter()
{
    Token token = lastToken();

    auto startExpr = parseExpression(nextToken(), true);
    if (!startExpr)
        error(mExpressionError);

    expectComma(lastTokenId());

    auto sizeExpr = parseExpression(nextToken(), true);
    if (!sizeExpr)
        error(mExpressionError);

    expectEol(lastTokenId());

    mContext->codeEmitter()->emit<WriteDirective>(token, std::move(startExpr), std::move(sizeExpr), ProgramWriteProtection::What::PopAllowWrite);
}

void AssemblerParser::parseAssertBank()
{
    Token token = lastToken();

    auto expr = parseExpression(nextToken(), true);
    if (!expr)
        error(mExpressionError);

    expectEol(lastTokenId());

    mContext->codeEmitter()->emit<WriteDirective>(token, std::move(expr), nullptr, ProgramWriteProtection::What::AssertBank);
}

void AssemblerParser::parseDefByte()
{
    do {
        auto token = (nextToken(), lastToken());
        if (token.id == T_STRING) {
            std::string text = lastTokenText();
            if (!text.empty())
                mContext->codeEmitter()->emit<DEFB_STRING>(token, std::move(text));
            nextToken();
        } else {
            auto expr = parseExpression(token.id, true);
            if (!expr)
                error(mExpressionError);
            mContext->codeEmitter()->emit<DEFB>(token, std::move(expr));
        }
    } while (lastTokenId() == T_COMMA);
    expectEol(lastTokenId());
}

void AssemblerParser::parseDefWord()
{
    do {
        auto token = (nextToken(), lastToken());
        auto expr = parseExpression(token.id, true);
        if (!expr)
            error(mExpressionError);
        mContext->codeEmitter()->emit<DEFW>(token, std::move(expr));
    } while (lastTokenId() == T_COMMA);
    expectEol(lastTokenId());
}

void AssemblerParser::parseDefDWord()
{
    do {
        auto token = (nextToken(), lastToken());
        auto expr = parseExpression(token.id, true);
        if (!expr)
            error(mExpressionError);
        mContext->codeEmitter()->emit<DEFD>(token, std::move(expr));
    } while (lastTokenId() == T_COMMA);
    expectEol(lastTokenId());
}

void AssemblerParser::parseDefSpace()
{
    auto token = (nextToken(), lastToken());
    auto expr = parseExpression(token.id, true);
    if (!expr)
        error(mExpressionError);
    mContext->codeEmitter()->emit<DEFS>(token, std::move(expr));
    expectEol(lastTokenId());
}
*/

Instruction* AssemblerParser::parseOpcode()
{
    SourceLocation* location = mToken->location();
    const char* matchedOpcode = nullptr;

    #define Z80_OPCODE_0(OP, BYTES, TSTATES) \
        { \
            ParsingContext context(mHeap, mToken, mSymbolTable); \
            if (Z80::Mnemonic::OP::tryParse(&context)) { \
                if (Z80::OP::tryParse(&context)) \
                    return new (mHeap) Z80::OP(location); \
                matchedOpcode = #OP; \
            } \
        }

    #define Z80_OPCODE_1(OP, OP1, BYTES, TSTATES) \
        { \
            ParsingContext context(mHeap, mToken, mSymbolTable); \
            if (Z80::Mnemonic::OP::tryParse(&context)) { \
                Z80::OP1 op1; \
                if (Z80::OP##_##OP1::tryParse(&context, op1)) \
                    return new (mHeap) Z80::OP##_##OP1(location, op1); \
                matchedOpcode = #OP; \
            } \
        }

    #define Z80_OPCODE_2(OP, OP1, OP2, BYTES, TSTATES) \
        { \
            ParsingContext context(mHeap, mToken, mSymbolTable); \
            if (Z80::Mnemonic::OP::tryParse(&context)) { \
                Z80::OP1 op1; \
                Z80::OP2 op2; \
                if (Z80::OP##_##OP1##_##OP2::tryParse(&context, op1, op2)) \
                    return new (mHeap) Z80::OP##_##OP1##_##OP2(location, op1, op2); \
                matchedOpcode = #OP; \
            } \
        }

    #include "Instructions.Z80.hh"

    #undef Z80_OPCODE_2
    #undef Z80_OPCODE_1
    #undef Z80_OPCODE_0

    if (matchedOpcode) {
        std::stringstream ss;
        ss << "invalid operands for opcode '" << matchedOpcode << "'.";
        throw CompilerError(location, ss.str());
    }

    return nullptr;
}

std::string AssemblerParser::readLabelName()
{
    switch (mToken->id()) {
        case TOK_LABEL_GLOBAL: {
            mContext->setLocalLabelsPrefix(mToken->location(), mToken->text());
            return mToken->text();
        }

        case TOK_LABEL_FULL:
            return mToken->text();

        case TOK_LABEL_LOCAL:
        case TOK_LABEL_LOCAL_NAME: {
            const auto& prefix = mContext->localLabelsPrefix();
            if (prefix.empty())
                throw CompilerError(mToken->location(), "local label name without preceding global label");

            std::stringstream ss;
            ss << prefix;
            ss << "@@";
            ss << mToken->text();
            return ss.str();
        }
    }

    assert(false);
    throw CompilerError(mToken->location(), "internal compiler error: invalid label token.");
}

const char* AssemblerParser::consumeIdentifier()
{
    if (mToken->id() < TOK_IDENTIFIER) {
        std::stringstream ss;
        ss << "unexpected " << mToken->name() << '.';
        throw CompilerError(mToken->location(), ss.str());
    }

    const char* text = mToken->text();
    mToken = mToken->next();

    return text;
}

/*
void AssemblerParser::expectComma(int tokenId)
{
    if (tokenId != T_COMMA)
        error(tr("expected ','"));
}
*/

void AssemblerParser::expectEol()
{
    if (!matchEol()) {
        std::stringstream ss;
        ss << "unexpected " << mToken->name() << '.';
        throw CompilerError(mToken->location(), ss.str());
    }
}

bool AssemblerParser::matchEol() const
{
    return (mToken->id() == TOK_EOF || mToken->isFirstOnLine());
}

/*
bool AssemblerParser::matchToken(int token)
{
    if (lastTokenId() != token)
        return false;
    nextToken();
    return true;
}

bool AssemblerParser::matchIdentifier(const char* ident)
{
    if (lastTokenId() < TOK_IDENTIFIER || toLower(lastTokenText()) != ident)
        return false;
    nextToken();
    return true;
}

bool AssemblerParser::matchExpression(std::unique_ptr<Expression>* out, bool unambiguous)
{
    return tryParseExpression(lastTokenId(), out, unambiguous);
}

bool AssemblerParser::matchExpressionNegative(const Token& minusToken, std::unique_ptr<Expression>* out)
{
    if (!matchExpression(out))
        return false;
    out->reset(new NegateExpression(minusToken, std::move(*out)));
    return true;
}

bool AssemblerParser::matchByte(quint8* out)
{
    std::unique_ptr<Expression> expr;
    if (!matchExpression(&expr))
        return false;

    ExprEvalContext context(mProgram, mReporter);
    *out = context.evaluateByte(expr);

    return true;
}
*/
