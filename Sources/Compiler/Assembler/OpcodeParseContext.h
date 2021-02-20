#ifndef COMPILER_ASSEMBLER_OPCODEPARSECONTEXT_H
#define COMPILER_ASSEMBLER_OPCODEPARSECONTEXT_H

class OpcodeParseContext
{
public:
    void advance();
    bool consumeComma();
    bool consumeIdentifier(const char* name);
    bool checkEnd();
};

#endif
