#include "Token.h"

Token* Token::append(Token* token)
{
    Token* p = this;
    while (p->mNext)
        p = p->mNext;

    p->mNext = token;
    while (p->mNext)
        p = p->mNext;

    return p;
}
