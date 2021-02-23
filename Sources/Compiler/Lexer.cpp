#include "Lexer.h"
#include "Compiler/LexerUtils.h"
#include "Compiler/CompilerError.h"
#include "Compiler/Token.h"
#include <sstream>
#include <string.h>

Lexer::Lexer(GCHeap* heap)
    : mHeap(heap)
    , mFirstToken(nullptr)
    , mLastToken(nullptr)
{
}

Lexer::~Lexer()
{
}

void Lexer::scan(const FileID* file, const char* p)
{
    mFile = file;
    mLine = 1;
    mLineStart = true;

    for (;;) {
        const char* start = p;
        mStartLine = mLine;

        switch (*p) {
            case 0: {
                token(TOK_EOF, "end of file");
                mLastToken->setNextToItself();
                return;
            }

            case ' ':
            case '\t':
            case '\r':
            case '\v':
            case '\f':
                ++p;
                continue;

            case '\n':
                ++p;
                ++mLine;
                mLineStart = true;
                continue;

            case ',':
                ++p;
                token(TOK_COMMA, "','");
                continue;

            case ';':
                ++p;
                token(TOK_SEMICOLON, "';'");
                continue;

            case ':':
                ++p;
                token(TOK_COLON, "':'");
                continue;

            case '?':
                ++p;
                token(TOK_QUESTION, "'?'");
                continue;

            case '^':
                ++p;
                token(TOK_CARET, "'^'");
                continue;

            case '~':
                ++p;
                token(TOK_TILDE, "'~'");
                continue;

            case '(':
                ++p;
                token(TOK_LPAREN, "'('");
                continue;

            case ')':
                ++p;
                token(TOK_RPAREN, "')'");
                continue;

            case '[':
                ++p;
                token(TOK_LBRACKET, "'['");
                continue;

            case ']':
                ++p;
                token(TOK_RBRACKET, "']'");
                continue;

            case '{':
                ++p;
                token(TOK_LCURLY, "'{'");
                continue;

            case '}':
                ++p;
                token(TOK_RCURLY, "'}'");
                continue;

            case '&':
                ++p;
                if (*p == '&') {
                    ++p;
                    token(TOK_DOUBLEAMPERSAND, "'&&'");
                } else
                    token(TOK_AMPERSAND, "'&'");
                continue;

            case '|':
                ++p;
                if (*p == '|') {
                    ++p;
                    token(TOK_DOUBLEVBAR, "'||'");
                } else
                    token(TOK_VBAR, "'|'");
                continue;

            case '=':
                ++p;
                if (*p == '=') {
                    ++p;
                    token(TOK_EQ, "'=='");
                } else
                    token(TOK_ASSIGN, "'='");
                continue;

            case '!':
                ++p;
                if (*p == '=') {
                    ++p;
                    token(TOK_INEQ, "'!='");
                } else
                    token(TOK_EXCLAMATION, "'!'");
                continue;

            case '<':
                ++p;
                if (*p == '<') {
                    ++p;
                    if (*p == '=') {
                        ++p;
                        token(TOK_SHLEQ, "'<<='");
                    } else
                        token(TOK_SHL, "'<<'");
                } else if (*p == '=') {
                    ++p;
                    token(TOK_LESSEQ, "'<='");
                } else
                    token(TOK_LESS, "'<'");
                continue;

            case '>':
                ++p;
                if (*p == '>') {
                    ++p;
                    if (*p == '=') {
                        ++p;
                        token(TOK_SHREQ, "'>>='");
                    } else
                        token(TOK_SHR, "'>'");
                } else if (*p == '=') {
                    ++p;
                    token(TOK_GREATEREQ, "'>='");
                } else
                    token(TOK_GREATER, "'>'");
                continue;

            case '+':
                ++p;
                if (*p == '=') {
                    ++p;
                    token(TOK_ADDEQ, "'+='");
                } else
                    token(TOK_PLUS, "'+'");
                continue;

            case '-':
                ++p;
                if (*p == '=') {
                    ++p;
                    token(TOK_SUBEQ, "'-='");
                } else
                    token(TOK_MINUS, "'-'");
                continue;

            case '*':
                ++p;
                if (*p == '=') {
                    ++p;
                    token(TOK_MULEQ, "'*='");
                } else
                    token(TOK_ASTERISK, "'*'");
                continue;

            case '%':
                ++p;
                if (*p == '=') {
                    ++p;
                    token(TOK_MODEQ, "'%='");
                } else
                    token(TOK_PERCENT, "'%'");
                continue;

            case '/':
                ++p;
                if (*p == '*') {
                    ++p;
                    for (;;) {
                        if (!*p)
                            throw CompilerError(location(), "unterminated comment.");
                        else if (*p == '\n')
                            ++mLine;
                        else if (*p == '*' && p[1] == '/') {
                            p += 2;
                            break;
                        }
                        ++p;
                    }
                    continue;
                } else if (*p == '/') {
                    ++p;
                    while (*p && *p != '\n')
                        ++p;
                    continue;
                } else if (*p == '=') {
                    ++p;
                    token(TOK_DIVEQ, "'/='");
                } else
                    token(TOK_SLASH, "'/'");
                continue;

            case '@': {
                ++p;
                if (*p == '@') {
                    ++p;
                    std::stringstream ss;
                    while (isIdentifier(*p))
                        ss << *p++;
                    std::string str = ss.str();
                    if (*p == ':') {
                        ++p;
                        token(TOK_LABEL_LOCAL, "label", str.c_str(), str.length());
                    }
                    token(TOK_IDENTIFIER, "label name", str.c_str(), str.length());
                    continue;
                }
                token(TOK_AT, "'@'");
                continue;
            }

            case '$': {
                ++p;
                token(TOK_DOLLAR, "'$'");
                continue;
            }

            case '#': {
                ++p;
                token(TOK_HASH, "'#'");
                continue;
            }

            case '0':
                if (p[1] == 'x' || p[1] == 'X') {
                    uint64_t value = 0;
                    p += 2;
                    if (!isHexDigit(*p))
                        throw CompilerError(location(), "syntax error in hexadecimal number.");
                    do {
                        if (value >= 0x0ffffffful)
                            throw CompilerError(location(), "hexadecimal number is too large.");
                        value <<= 4;
                        value |= charToInt(*p++);
                    } while (isHexDigit(*p));
                    token(TOK_NUMBER, "hexadecimal number", value);
                    continue;
                }
                if (p[1] == 'b' || p[1] == 'B') {
                    uint64_t value = 0;
                    p += 2;
                    if (!isBinaryDigit(*p))
                        throw CompilerError(location(), "syntax error in binary number.");
                    do {
                        if (value >= 0x7ffffffful)
                            throw CompilerError(location(), "binary number is too large.");
                        value <<= 1;
                        value |= charToInt(*p++);
                    } while (isBinaryDigit(*p));
                    token(TOK_NUMBER, "binary number", value);
                    continue;
                }
                /* falls through. */
            case DIGIT_1_9: {
                uint64_t value = charToInt(*p);
                do {
                    if (value >= 0xfffffffful / 10)
                        throw CompilerError(location(), "number is too large.");
                    value *= 10;
                    value |= charToInt(*p++);
                } while (isDigit(*p));
                token(TOK_NUMBER, "number", value);
                continue;
            }

            case LETTER:
            case '_': {
                ++p;
                bool hadAt = false;
                for (;;) {
                    if (isIdentifier(*p))
                        ++p;
                    else if (*p == '@' && p[1] == '@' && !hadAt) {
                        p += 2;
                        hadAt = true;
                        continue;
                    }
                    else
                        break;
                }
                size_t len = (size_t)(p - start);
                if (*p == '\'' && len == 2 && (*start == 'a' || *start == 'A') && (start[1] == 'f' || start[1] == 'F')) {
                    ++p;
                    token(TOK_IDENTIFIER, "identifier", start, len);
                    continue;
                }
                if (*p == ':') {
                    ++p;
                    if (hadAt)
                        token(TOK_LABEL_FULL, "label", start, len);
                    else
                        token(TOK_LABEL_GLOBAL, "label", start, len);
                    continue;
                }
                #define KEYWORD(NAME, KW) \
                    if (len == sizeof(NAME)-1 && !memcmp(start, NAME, len)) { \
                        keyword(KW, "'" NAME "'", #NAME); \
                        continue; \
                    }
                switch (*start) {
                    case 'b':
                        KEYWORD("bool", KW_BOOL)
                        KEYWORD("byte", KW_BYTE)
                        break;
                    case 'c':
                        KEYWORD("const", KW_CONST)
                        break;
                    case 'd':
                        KEYWORD("dword", KW_DWORD)
                        break;
                    case 'e':
                        KEYWORD("else", KW_ELSE)
                        break;
                    case 'f':
                        KEYWORD("false", KW_FALSE)
                        break;
                    case 'i':
                        KEYWORD("if", KW_IF)
                        KEYWORD("int", KW_INT)
                        break;
                    case 'l':
                        KEYWORD("long", KW_LONG)
                        break;
                    case 'n':
                        KEYWORD("null", KW_NULL)
                        break;
                    case 's':
                        KEYWORD("sbyte", KW_SBYTE)
                        KEYWORD("string", KW_STRING)
                        break;
                    case 't':
                        KEYWORD("true", KW_TRUE)
                        break;
                    case 'v':
                        KEYWORD("void", KW_VOID)
                        break;
                    case 'w':
                        KEYWORD("word", KW_WORD)
                        break;
                }
                token(TOK_IDENTIFIER, "identifier", start, len);
                continue;
            }

            case '"':
            case '\'': {
                char ch = *p++;
                std::stringstream ss;
                while (*p != ch) {
                    switch (*p) {
                        case 0:
                        case '\n':
                            throw CompilerError(location(), "unterminated string literal.");
                            break;
                        case '\\':
                            ++p;
                            switch (*p) {
                                case 0:
                                    throw CompilerError(location(), "unterminated string literal.");
                                    break;
                                case '\n':
                                    ++mLine;
                                    /* falls through. */
                                case '\\':
                                case '\'':
                                case '"':
                                    ss << *p++;
                                    break;
                                case 'a': ++p; ss << '\a'; break;
                                case 'b': ++p; ss << '\b'; break;
                                case 'e': ++p; ss <<   27; break;
                                case 'f': ++p; ss << '\f'; break;
                                case 'n': ++p; ss << '\n'; break;
                                case 'r': ++p; ss << '\r'; break;
                                case 't': ++p; ss << '\t'; break;
                                case 'v': ++p; ss << '\v'; break;
                                default:
                                    throw CompilerError(location(), "invalid escape sequence.");
                                    break;
                            }
                            break;
                        default:
                            ss << *p++;
                    }
                }
                ++p;
                std::string str = ss.str();
                if (ch == '"')
                    token(TOK_STRING, "string literal", str.c_str(), str.length());
                else {
                    if (str.length() == 0)
                        throw CompilerError(location(), "empty character literal.");
                    else if (str.length() != 1)
                        throw CompilerError(location(), "character literal is too long.");
                    token(TOK_CHAR, "character literal", (unsigned char)str[0]);
                }
                continue;
            }

            default: {
                std::stringstream ss;
                if (isprint(*p))
                    ss << "unexpected character '" << *p << "'.";
                else
                    ss << "unexpected symbol with code " << static_cast<unsigned char>(*p) << '.';
                throw CompilerError(location(), ss.str());
                continue;
            }
        }
    }
}

SourceLocation* Lexer::location()
{
    return new (mHeap) SourceLocation(mFile, mLine);
}

void Lexer::token(TokenID id, const char* name)
{
    appendToken(new (mHeap) Token(location(), id, name));
}

void Lexer::token(TokenID id, const char* name, const char* text, size_t length)
{
    appendToken(new (mHeap) Token(location(), id, name, mHeap->allocString(text, length)));
}

void Lexer::token(TokenID id, const char* name, uint64_t number)
{
    appendToken(new (mHeap) Token(location(), id, name, number));
}

void Lexer::keyword(TokenID id, const char* name, const char* text)
{
    appendToken(new (mHeap) Token(location(), id, name, text));
}

void Lexer::appendToken(Token* token)
{
    if (mLineStart) {
        token->setFirstOnLine();
        mLineStart = false;
    }

    if (mLastToken)
        mLastToken = mLastToken->append(token);
    else {
        mFirstToken = token;
        mLastToken = token;
    }
}
