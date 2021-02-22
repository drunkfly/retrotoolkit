#ifndef COMPILER_ASSEMBLERPARSER_H
#define COMPILER_ASSEMBLERPARSER_H

#include "Common/Common.h"
/*
#include <unordered_map>
#include <memory>
*/

struct Token;
class GCHeap;
class Instruction;

class AssemblerParser
{
public:
    explicit AssemblerParser(GCHeap* heap);
    ~AssemblerParser();

    void parse(const Token* tokens);

private:
    GCHeap* mHeap;
    const Token* mToken;

    /*
    QString mExpressionError;
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

    std::unique_ptr<Expression> parseAtomicExpression(int tokenId, bool unambiguous); // in ExpressionParser.cpp
    std::unique_ptr<Expression> parseUnaryExpression(int tokenId, bool unambiguous); // in ExpressionParser.cpp
    std::unique_ptr<Expression> parseMultiplicationExpression(int tokenId, bool unambiguous); // in ExpressionParser.cpp
    std::unique_ptr<Expression> parseAdditionExpression(int tokenId, bool unambiguous); // in ExpressionParser.cpp
    std::unique_ptr<Expression> parseShiftExpression(int tokenId, bool unambiguous); // in ExpressionParser.cpp
    std::unique_ptr<Expression> parseRelationalExpression(int tokenId, bool unambiguous); // in ExpressionParser.cpp
    std::unique_ptr<Expression> parseEqualityExpression(int tokenId, bool unambiguous); // in ExpressionParser.cpp
    std::unique_ptr<Expression> parseBitwiseAndExpression(int tokenId, bool unambiguous); // in ExpressionParser.cpp
    std::unique_ptr<Expression> parseBitwiseXorExpression(int tokenId, bool unambiguous); // in ExpressionParser.cpp
    std::unique_ptr<Expression> parseBitwiseOrExpression(int tokenId, bool unambiguous); // in ExpressionParser.cpp
    std::unique_ptr<Expression> parseLogicalAndExpression(int tokenId, bool unambiguous); // in ExpressionParser.cpp
    std::unique_ptr<Expression> parseLogicalOrExpression(int tokenId, bool unambiguous); // in ExpressionParser.cpp
    std::unique_ptr<Expression> parseConditionalExpression(int tokenId, bool unambiguous); // in ExpressionParser.cpp
    std::unique_ptr<Expression> parseExpression(int tokenId, bool unambiguous); // in ExpressionParser.cpp
    bool tryParseExpression(int tokenId, std::unique_ptr<Expression>* out, bool unambiguous); // in ExpressionParser.cpp

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

    int nextToken() const;
    const Token& lastToken() const;
    int lastTokenId() const;
    const std::string& lastTokenText() const;
    const char* lastTokenCStr() const;

    void error(const QString& message);
    void error(const Token& token, const QString& message);
    */

    DISABLE_COPY(AssemblerParser);
};

#endif
