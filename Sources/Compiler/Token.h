#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H

#include "Common/GC.h"
#include <stdint.h>

enum TokenID
{
    TOK_EOF,
    TOK_NUMBER,
    TOK_LABEL_FULL,
    TOK_LABEL_GLOBAL,
    TOK_LABEL_LOCAL,
    TOK_CHAR,
    TOK_STRING,
    TOK_COMMA,
    TOK_SEMICOLON,
    TOK_COLON,
    TOK_QUESTION,
    TOK_CARET,
    TOK_TILDE,
    TOK_EXCLAMATION,
    TOK_PLUS,
    TOK_MINUS,
    TOK_ASTERISK,
    TOK_PERCENT,
    TOK_ASSIGN,
    TOK_AT,
    TOK_DOLLAR,
    TOK_HASH,
    TOK_SLASH,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_LCURLY,
    TOK_RCURLY,
    TOK_AMPERSAND,
    TOK_DOUBLEAMPERSAND,
    TOK_VBAR,
    TOK_DOUBLEVBAR,
    TOK_SHL,
    TOK_SHR,
    TOK_EQ,
    TOK_INEQ,
    TOK_LESS,
    TOK_LESSEQ,
    TOK_GREATER,
    TOK_GREATEREQ,
    TOK_ADDEQ,
    TOK_SUBEQ,
    TOK_MULEQ,
    TOK_DIVEQ,
    TOK_MODEQ,
    TOK_SHLEQ,
    TOK_SHREQ,
    TOK_IDENTIFIER,
    KW_BOOL,
    KW_BYTE,
    KW_CONST,
    KW_DWORD,
    KW_ELSE,
    KW_FALSE,
    KW_IF,
    KW_INT,
    KW_LONG,
    KW_NULL,
    KW_SBYTE,
    KW_STRING,
    KW_TRUE,
    KW_VOID,
    KW_WORD,
};

class SourceLocation;

class Token : public GCObject
{
public:
    Token(SourceLocation* location, TokenID id, const char* name)
        : mID(id)
        , mNext(nullptr)
        , mLocation(location)
        , mName(name)
        , mFirstOnLine(false)
    {
    }

    Token(SourceLocation* location, TokenID id, const char* name, const char* text)
        : Token(location, id, name)
    {
        mValue.text = text;
    }

    Token(SourceLocation* location, TokenID id, const char* name, uint64_t number)
        : Token(location, id, name)
    {
        mValue.number = number;
    }

    TokenID id() const { return mID; }
    Token* next() const { return mNext; }
    SourceLocation* location() const { return mLocation; }
    const char* name() const { return mName; }
    const char* text() const { return mValue.text; }
    uint64_t number() const { return mValue.number; }

    bool isFirstOnLine() const { return mFirstOnLine; }
    void setFirstOnLine() { mFirstOnLine = true; }

    void setNextToItself() { mNext = this; }

    Token* append(Token* token);

private:
    TokenID mID;
    Token* mNext;
    SourceLocation* mLocation;
    const char* mName;
    union {
        const char* text;
        uint64_t number;
    } mValue;
    bool mFirstOnLine;

    DISABLE_COPY(Token);
};

#endif
