#ifndef ASSEMBLER_OPCODEPARSECONTEXT_H
#define ASSEMBLER_OPCODEPARSECONTEXT_H

class OpcodeParseContext
{
public:
    void advance();
    bool consumeComma();
    bool consumeIdentifier(const char* name);
    bool checkEnd();
};

#endif
