#include "ErrorConsumer.h"
#include "Compiler/Tree/SourceLocation.h"
#include <sstream>

ErrorConsumer::ErrorConsumer()
{
}

ErrorConsumer::~ErrorConsumer()
{
}

void ErrorConsumer::setError(const CompilerError& error)
{
    mError = std::make_unique<CompilerError>(error);
}

std::string ErrorConsumer::errorMessage() const
{
    if (!mError)
        return std::string();

    std::stringstream ss;
    auto file = mError->location()->file();
    if (file) {
        ss << file->name().string();
        ss << ':';
    }
    ss << mError->location()->line();
    ss << ": ";
    ss << mError->message();
    return ss.str();
}