#include "CompilerError.h"
#include <sstream>

CompilerError::CompilerError(SourceLocation* location, std::string message)
    : mLocation(location)
    , mMessage(std::move(message))
    , mFullMessage(makeFullMessage(mLocation, mMessage))
{
}

CompilerError::~CompilerError() noexcept
{
}

const char* CompilerError::what() const noexcept
{
    return mFullMessage.c_str();
}

std::string CompilerError::makeFullMessage(SourceLocation* location, const std::string& message)
{
    std::stringstream ss;
    if (location && location->file()) {
        if (location->line() <= 0)
            ss << location->file()->path().string() << ": ";
        else
            ss << location->file()->path().string() << '(' << location->line() << "): ";
    }
    ss << message;
    return ss.str();
}
