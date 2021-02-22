#ifndef COMPILER_ASSEMBLER_OPCODEPARSECONTEXT_H
#define COMPILER_ASSEMBLER_OPCODEPARSECONTEXT_H

#include "Common/Common.h"

class Token;

class OpcodeParseContext
{
public:
    explicit OpcodeParseContext(const Token*& token)
        : mToken(token)
        , mTokenRef(token)
    {
    }

    bool consumeComma();
    bool consumeIdentifier(const char* name);
    bool checkEnd();

private:
    const Token* mToken;
    const Token*& mTokenRef;

    DISABLE_COPY(OpcodeParseContext);
};

#endif
