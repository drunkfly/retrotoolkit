#include "CompilerError.h"
#include <sstream>

CompilerError::CompilerError(SourceLocation* location, std::string message)
    : mLocation(location)
    , mMessage(std::move(message))
{
    std::stringstream ss;
    if (location->file() && location->line() > 0)
        ss << location->file() << '(' << location->line() << "): ";
    ss << message;
    mFullMessage = ss.str();
}

CompilerError::~CompilerError() noexcept
{
}

const char* CompilerError::what() const noexcept
{
    return mFullMessage.c_str();
}
