#include "OpcodeParseContext.h"
#include "Common/Strings.h"
#include "Compiler/Token.h"
#include <ctype.h>

bool OpcodeParseContext::consumeComma()
{
    if (mToken->id() != TOK_COMMA)
        return false;
    mToken = mToken->next();
    return true;
}

bool OpcodeParseContext::consumeIdentifier(const char* name)
{
    if (mToken->id() < TOK_IDENTIFIER)
        return false;
    if (!equalCaseInsensitive(mToken->text(), name))
        return false;
    mToken = mToken->next();
    return true;
}

bool OpcodeParseContext::checkEnd()
{
    if (mToken->id() != TOK_EOF && !mToken->isFirstOnLine())
        return false;
    mTokenRef = mToken;
    return true;
}
