#ifndef COMPILER_COMPILERERROR_H
#define COMPILER_COMPILERERROR_H

#include "Compiler/Tree/SourceLocation.h"

class CompilerError : public std::exception
{
public:
    CompilerError(SourceLocation* location, std::string message);
    ~CompilerError() noexcept override;

    SourceLocation* location() const noexcept { return mLocation; }
    const std::string& message() const noexcept { return mMessage; }
    const std::string& fullMessage() const noexcept { return mFullMessage; }

    const char* what() const noexcept override;

    static void locationToString(SourceLocation* location, const char* suffix, std::stringstream& ss);
    static std::string makeFullMessage(SourceLocation* location, const std::string& message);

private:
    SourceLocation* mLocation;
    std::string mMessage;
    std::string mFullMessage;
};

#endif
