#ifndef COMPILER_ASSEMBLER_ASSEMBLERPARSER_H
#define COMPILER_ASSEMBLER_ASSEMBLERPARSER_H

#include "Common/Common.h"
/*
#include <unordered_map>
#include <memory>
*/

struct Token;
class GCHeap;
class SymbolTable;
class Instruction;

class AssemblerParser
{
public:
    AssemblerParser(GCHeap* heap, SymbolTable* globals);
    ~AssemblerParser();

    void parse(const Token* tokens);

private:
    GCHeap* mHeap;
    SymbolTable* mGlobals;
    SymbolTable* mSymbolTable;
    const Token* mToken;

    /*
    std::unique_ptr<AssemblerContext> mContext;
    static std::unordered_map<std::string, void(AssemblerParser::*)()> mDataDirectives;
    static std::unordered_map<std::string, void(AssemblerParser::*)()> mDirectives;

    template <typename T, typename... ARGS> T* pushContext(ARGS&&... args);
    void popContext();
    */

    void parseLine();

    /*
    void parseSectionDecl();
    void parseRepeatDecl();
    void parseEndRepeatDecl();
    void parseIfDecl();
    void parseElseDecl();
    void parseEndIfDecl();
    void parseAllowWrite();
    void parseDisallowWrite();
    void parsePushAllowWrite();
    void parsePushDisallowWrite();
    void parsePopAllowWrite();
    void parsePopAllowWriteAfter();
    void parseAssertBank();

    void parseDefByte();
    void parseDefWord();
    void parseDefDWord();
    void parseDefSpace();
    */

    Instruction* parseOpcode();

    /*
    std::string readLabelName(int tokenId);

    std::string expectIdentifier(int tokenId);
    void expectComma(int tokenId);
    void expectEol(int tokenId);
    */

    bool matchEol() const;
    /*
    bool matchToken(int token);
    bool matchIdentifier(const char* ident);
    bool matchExpression(std::unique_ptr<Expression>* out, bool unambiguous = false);
    bool matchExpressionNegative(const Token& minusToken, std::unique_ptr<Expression>* out);
    bool matchByte(quint8* out);
    */

    DISABLE_COPY(AssemblerParser);
};

#endif
