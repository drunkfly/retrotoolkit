#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include "Common/Common.h"
#include "Compiler/Token.h"

class Lexer
{
public:
    explicit Lexer(GCHeap* heap);
    ~Lexer();

    Token* firstToken() const { return mFirstToken; }

    void scan(const char* file, const char* p);

private:
    GCHeap* mHeap;
    Token* mFirstToken;
    Token* mLastToken;
    const char* mFile;
    const char* mStart;
    int mStartLine;
    int mLine;

    SourceLocation* location();

    void token(TokenID id, const char* name);
    void token(TokenID id, const char* name, const char* text, size_t length);
    void token(TokenID id, const char* name, uint64_t number);
    void appendToken(Token* token);

    DISABLE_COPY(Lexer);
};

#endif
