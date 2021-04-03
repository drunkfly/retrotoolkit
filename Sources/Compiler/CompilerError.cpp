#include "CompilerError.h"

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

void CompilerError::locationToString(SourceLocation* location, const char* suffix, std::stringstream& ss)
{
    if (location && location->file()) {
        if (location->line() <= 0)
            ss << location->file()->name().string() << suffix;
        else
            ss << location->file()->name().string() << '(' << location->line() << ')' << suffix;
    }
}

std::string CompilerError::makeFullMessage(SourceLocation* location, const std::string& message)
{
    std::stringstream ss;
    locationToString(location, ": ", ss);
    ss << message;
    return ss.str();
}
