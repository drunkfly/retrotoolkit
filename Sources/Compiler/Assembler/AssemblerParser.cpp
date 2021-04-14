#include "AssemblerParser.h"
#include "Compiler/Token.h"
#include "Compiler/ParsingContext.h"
#include "Compiler/CompilerError.h"
#include "Compiler/Assembler/Instructions.Z80.h"
#include "Compiler/Assembler/DataDirectives.h"
#include "Compiler/Assembler/AssemblerContext.h"
#include "Compiler/Assembler/AssemblerContextRepeat.h"
#include "Compiler/Assembler/AssemblerContextIf.h"
#include "Compiler/Assembler/MacroEnsure.h"
#include "Compiler/Assembler/Label.h"
#include "Compiler/Tree/Symbol.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/Linker/Program.h"
#include "Compiler/Linker/ProgramSection.h"
#include "Common/GC.h"
#include "Common/Strings.h"

const std::unordered_map<std::string, void(AssemblerParser::*)()> AssemblerParser::mDataDirectives = {
        { "db", &AssemblerParser::parseDefByte },
        { "dw", &AssemblerParser::parseDefWord },
        { "dd", &AssemblerParser::parseDefDWord },
        { "defb", &AssemblerParser::parseDefByte },
        { "defm", &AssemblerParser::parseDefByte },
        { "defw", &AssemblerParser::parseDefWord },
        { "defs", &AssemblerParser::parseDefSpace },
    };

const std::unordered_map<std::string, void(AssemblerParser::*)()> AssemblerParser::mDirectives = {
        { "section", &AssemblerParser::parseSectionDecl },
        { "repeat", &AssemblerParser::parseRepeatDecl },
        { "endrepeat", &AssemblerParser::parseEndRepeatDecl },
        { "if", &AssemblerParser::parseIfDecl },
        { "else", &AssemblerParser::parseElseDecl },
        { "endif", &AssemblerParser::parseEndIfDecl },
        { "ensure", &AssemblerParser::parseEnsureDecl },
        /*
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

    if (mContext->prev()) {
        if (mContext->isRepeat())
            throw CompilerError(mToken->location(), "missing 'endrepeat'.");
        else if (mContext->isIf())
            throw CompilerError(mToken->location(), "missing 'endif'.");
        else
            throw CompilerError(mToken->location(), "internal compiler error: context stack is not empty.");
    }
}

template <typename T, typename... ARGS> T* AssemblerParser::pushContext(ARGS&&... args)
{
    T* context = new (mHeap) T(mContext, std::forward<ARGS>(args)...);
    mContext = context;
    return context;
}

void AssemblerParser::popContext()
{
    if (!mContext)
        throw CompilerError(mToken->location(), "internal compiler error: context stack is empty.");

    mContext->validateAtPop(mToken->location());

    mContext = mContext->prev();

    if (!mContext)
        throw CompilerError(mToken->location(), "internal compiler error: context stack is empty.");
}

void AssemblerParser::parseLine()
{
    // read label, if any
    if (mToken->id() == TOK_LABEL_GLOBAL || mToken->id() == TOK_LABEL_FULL || mToken->id() == TOK_LABEL_LOCAL) {
        std::string name = readLabelName();
        mContext->addLabel(mSymbolTable, mToken->location(), mHeap->allocString(name.c_str(), name.length()));
        mToken = mToken->next();

        // skip label-only lines
        if (matchEol())
            return;
    }

    // read #directive
    if (mToken->id() == TOK_HASH) {
        mToken = mToken->next();
        expectNotEol();

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

    // read directive / instruction
    if (mToken->id() >= TOK_IDENTIFIER) {
        Instruction* instruction = parseOpcode();
        if (instruction) {
            mContext->addInstruction(instruction);
            return;
        }

        std::string str = toLower(mToken->text());
        auto jt = mDataDirectives.find(str);
        if (jt != mDataDirectives.end()) {
            (this->*(jt->second))();
            return;
        }
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
        throw CompilerError(nameToken->location(), "expected opcode or directive.");

    // read directive
    mToken = mToken->next();
    std::string lower;
    if (mToken->id() >= TOK_IDENTIFIER)
        lower = toLower(mToken->text());
    std::unordered_map<std::string, void(AssemblerParser::*)()>::const_iterator iter;
    if (mToken->id() >= TOK_IDENTIFIER && (iter = mDataDirectives.find(lower)) != mDataDirectives.end()) {
        expectNotEol();

        mContext->addLabel(mSymbolTable, nameToken->location(), mHeap->allocString(name.c_str(), name.length()));
        if (nameToken->id() >= TOK_IDENTIFIER)
            mContext->setLocalLabelsPrefix(nameToken->location(), nameToken->text());

        (this->*(iter->second))();
    } else if ((mToken->id() >= TOK_IDENTIFIER && lower == "equ") || mToken->id() == TOK_ASSIGN) {
        expectNotEol();

        mToken = mToken->next();
        expectNotEol();

        const char* rawName = mHeap->allocString(name.c_str(), name.length());
        Expr* expr = ParsingContext(mHeap, mToken, mSymbolTable, &mContext->localLabelsPrefix(), false).unambiguousExpression();

        expr->replaceCurrentAddressWithLabel(mContext);

        mContext->addConstant(mSymbolTable, nameToken->location(), rawName, expr);
    } else {
        if (nameToken->id() >= TOK_IDENTIFIER) {
            std::stringstream ss;
            ss << "unknown opcode \"" << nameToken->text() << "\".";
            throw CompilerError(nameToken->location(), ss.str());
        }
        throw CompilerError(nameToken->location(), "missing ':' after local label name.");
    }
}

void AssemblerParser::parseSectionDecl()
{
    mToken = mToken->next();
    expectNotEol();

    SourceLocation* location = mToken->location();
    const char* sectionName = consumeIdentifier();
    auto section = mProgram->getOrAddSection(sectionName);

    if (!mContext->setCurrentSection(section))
        throw CompilerError(location, "'section' directive is not allowed in this context.");

    expectEol();
}

void AssemblerParser::parseRepeatDecl()
{
    const Token* token = mToken;

    mToken = mToken->next();
    expectNotEol();

    Expr* e = ParsingContext(mHeap, mToken, mSymbolTable, &mContext->localLabelsPrefix(), false).unambiguousExpression();

    std::string variable;

    if (mToken->id() == TOK_COMMA) {
        expectNotEol();

        mToken = mToken->next();
        expectNotEol();

        variable = consumeIdentifier();
    }

    mSymbolTable = new (mHeap) SymbolTable(mSymbolTable, true);
    pushContext<AssemblerContextRepeat>(token, mSymbolTable, std::move(variable), e);

    expectEol();
}

void AssemblerParser::parseEndRepeatDecl()
{
    if (!mContext->isRepeat())
        throw CompilerError(mToken->location(), "mismatched 'endrepeat'.");

    popContext();
    mSymbolTable = mSymbolTable->parent();

    mToken = mToken->next();
    expectEol();
}

void AssemblerParser::parseIfDecl()
{
    const Token* token = mToken;

    mToken = mToken->next();
    expectNotEol();

    Expr* e = ParsingContext(mHeap, mToken, mSymbolTable, &mContext->localLabelsPrefix(), false).unambiguousExpression();
    pushContext<AssemblerContextIf>(token, e);

    expectEol();
}

void AssemblerParser::parseElseDecl()
{
    if (!mContext->isIf() || mContext->hasElse())
        throw CompilerError(mToken->location(), "unexpected 'else'.");

    mContext->beginElse(mToken);
    mToken = mToken->next();

    expectEol();
}

void AssemblerParser::parseEndIfDecl()
{
    if (!mContext->isIf())
        throw CompilerError(mToken->location(), "mismatched 'endif'.");

    popContext();
    mToken = mToken->next();

    expectEol();
}

void AssemblerParser::parseEnsureDecl()
{
    mToken = mToken->next();
    expectNotEol();

    auto e = ParsingContext(mHeap, mToken, mSymbolTable, &mContext->localLabelsPrefix(), false).unambiguousExpression();
    mContext->addInstruction(new (mHeap) MacroEnsure(e->location(), e));

    expectEol();
}

/*
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
*/

void AssemblerParser::parseDefByte()
{
    do {
        mToken = mToken->next();
        expectNotEol();
        if (mToken->id() == TOK_STRING) {
            const char* text = mToken->text();
            if (*text)
                mContext->addInstruction(new (mHeap) DEFB_STRING(mToken->location(), text, strlen(text)));
            mToken = mToken->next();
        } else {
            auto e = ParsingContext(mHeap, mToken, mSymbolTable, &mContext->localLabelsPrefix(), false).unambiguousExpression();
            mContext->addInstruction(new (mHeap) DEFB(e->location(), e));
        }
    } while (mToken->id() == TOK_COMMA);
    expectEol();
}

void AssemblerParser::parseDefWord()
{
    do {
        mToken = mToken->next();
        expectNotEol();
        auto expr = ParsingContext(mHeap, mToken, mSymbolTable, &mContext->localLabelsPrefix(), false).unambiguousExpression();
        mContext->addInstruction(new (mHeap) DEFW(expr->location(), expr));
    } while (mToken->id() == TOK_COMMA);
    expectEol();
}

void AssemblerParser::parseDefDWord()
{
    do {
        mToken = mToken->next();
        expectNotEol();
        auto expr = ParsingContext(mHeap, mToken, mSymbolTable, &mContext->localLabelsPrefix(), false).unambiguousExpression();
        mContext->addInstruction(new (mHeap) DEFD(expr->location(), expr));
    } while (mToken->id() == TOK_COMMA);
    expectEol();
}

void AssemblerParser::parseDefSpace()
{
    mToken = mToken->next();
    expectNotEol();
    Expr* expr = ParsingContext(mHeap, mToken, mSymbolTable, &mContext->localLabelsPrefix(), false).unambiguousExpression();
    mContext->addInstruction(new (mHeap) DEFS(expr->location(), expr));
    expectEol();
}

Instruction* AssemblerParser::parseOpcode()
{
    SourceLocation* location = mToken->location();
    const char* matchedOpcode = nullptr;

    #define Z80_OPCODE_0(OP, BYTES, TSTATES) \
        { \
            ParsingContext context(mHeap, mToken, mSymbolTable, &mContext->localLabelsPrefix(), false); \
            if (Z80::Mnemonic::OP::tryParse(&context)) { \
                if (Z80::OP::tryParse(&context)) \
                    return new (mHeap) Z80::OP(location); \
                matchedOpcode = #OP; \
            } \
        }

    #define Z80_OPCODE_1(OP, OP1, BYTES, TSTATES) \
        { \
            ParsingContext context(mHeap, mToken, mSymbolTable, &mContext->localLabelsPrefix(), false); \
            if (Z80::Mnemonic::OP::tryParse(&context)) { \
                Z80::OP1 op1; \
                if (Z80::OP##_##OP1::tryParse(&context, op1)) \
                    return new (mHeap) Z80::OP##_##OP1(location, op1); \
                matchedOpcode = #OP; \
            } \
        }

    #define Z80_OPCODE_2(OP, OP1, OP2, BYTES, TSTATES) \
        { \
            ParsingContext context(mHeap, mToken, mSymbolTable, &mContext->localLabelsPrefix(), false); \
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
                throw CompilerError(mToken->location(), "local label name without preceding global label.");

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

void AssemblerParser::expectNotEol()
{
    if (mToken->isFirstOnLine()) {
        auto loc = mToken->location();
        auto file = (loc ? loc->file() : nullptr);
        int line = (loc && loc->line() > 1 ? loc->line() - 1 : 0);
        throw CompilerError(new (mHeap) SourceLocation(file, line), "unexpected end of line.");
    }
}

bool AssemblerParser::matchEol() const
{
    return (mToken->id() == TOK_EOF || mToken->isFirstOnLine());
}
