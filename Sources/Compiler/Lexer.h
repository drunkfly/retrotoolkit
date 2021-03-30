#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include "Compiler/Token.h"

class FileID;

class Lexer
{
public:
    enum class Mode
    {
        Assembler,
        SingleLineExpression,
    };

    Lexer(GCHeap* heap, Mode mode);
    ~Lexer();

    Token* firstToken() const { return mFirstToken; }

    void scan(const FileID* file, const char* p, int startLine = 1);

private:
    GCHeap* mHeap;
    Mode mMode;
    Token* mFirstToken;
    Token* mLastToken;
    const FileID* mFile;
    const char* mStart;
    int mStartLine;
    int mLine;
    bool mLineStart;

    SourceLocation* location();

    void token(TokenID id, const char* name);
    void token(TokenID id, const char* name, const char* text, size_t length);
    void token(TokenID id, const char* name, uint64_t number);
    void keyword(TokenID id, const char* name, const char* text);
    void appendToken(Token* token);

    DISABLE_COPY(Lexer);
};

#endif
