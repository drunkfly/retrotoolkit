#ifndef COMPILER_ASSEMBLER_ASSEMBLERPARSER_H
#define COMPILER_ASSEMBLER_ASSEMBLERPARSER_H

#include "Common/Common.h"

class GCHeap;
class AssemblerContext;
class Program;
class SymbolTable;
class Instruction;
class Token;

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

    static const std::unordered_map<std::string, void(AssemblerParser::*)()> mDataDirectives;
    static const std::unordered_map<std::string, void(AssemblerParser::*)()> mDirectives;

    template <typename T, typename... ARGS> T* pushContext(ARGS&&... args);
    void popContext();

    void parseLine();

    void parseSectionDecl();
    void parseRepeatDecl();
    void parseEndRepeatDecl();
    void parseIfDecl();
    void parseElseDecl();
    void parseEndIfDecl();
    void parseEnsureDecl();
    /*
    void parseAllowWrite();
    void parseDisallowWrite();
    void parsePushAllowWrite();
    void parsePushDisallowWrite();
    void parsePopAllowWrite();
    void parsePopAllowWriteAfter();
    void parseAssertBank();
    */

    void parseDefByte();
    void parseDefWord();
    void parseDefDWord();
    void parseDefSpace();

    Instruction* parseOpcode();

    std::string readLabelName();

    const char* consumeIdentifier();
    //void expectComma(int tokenId);
    void expectEol();
    void expectNotEol();

    bool matchEol() const;

    DISABLE_COPY(AssemblerParser);
};

#endif
