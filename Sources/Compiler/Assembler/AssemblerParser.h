#ifndef COMPILER_ASSEMBLER_ASSEMBLERPARSER_H
#define COMPILER_ASSEMBLER_ASSEMBLERPARSER_H

#include "Common/Common.h"
#include <unordered_map>
/*
#include <memory>
*/

struct Token;
class GCHeap;
class AssemblerContext;
class Program;
class SymbolTable;
class Instruction;

class AssemblerParser
{
public:
    AssemblerParser(GCHeap* heap, Program* program);
    ~AssemblerParser();

    void parse(const Token* tokens);

private:
    GCHeap* mHeap;
    AssemblerContext* mContext;
    Program* mProgram;
    SymbolTable* mSymbolTable;
    const Token* mToken;

    /*
    static std::unordered_map<std::string, void(AssemblerParser::*)()> mDataDirectives;
    */
    static std::unordered_map<std::string, void(AssemblerParser::*)()> mDirectives;

    /*
    template <typename T, typename... ARGS> T* pushContext(ARGS&&... args);
    void popContext();
    */

    void parseLine();

    void parseSectionDecl();
    /*
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

    std::string readLabelName();

    const char* consumeIdentifier();
    //void expectComma(int tokenId);
    void expectEol();

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
