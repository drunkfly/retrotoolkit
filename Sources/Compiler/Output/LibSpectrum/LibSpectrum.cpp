#include "LibSpectrum.h"
#include "Compiler/CompilerError.h"
#include <string.h>

std::mutex LibSpectrum::mutex;
std::vector<std::string> LibSpectrum::messages;

LibSpectrum::LibSpectrum()
    : lock_guard(mutex)
{
    messages.clear();

    libspectrum_error_function = errorFn;
    if (libspectrum_init() != LIBSPECTRUM_ERROR_NONE) {
        throwIfError();
        throw CompilerError(nullptr, "Unable to initialize libspectrum.");
    }
}

LibSpectrum::~LibSpectrum()
{
    libspectrum_end();
    libspectrum_error_function = libspectrum_default_error_function;
}

void LibSpectrum::throwIfError() const
{
    if (!messages.empty())
        throw CompilerError(nullptr, messages[0]);
}

libspectrum_error LibSpectrum::errorFn(libspectrum_error, const char* fmt, va_list args)
{
    static const char prefix[] = "libspectrum: ";

    char buf[2048];
    strcpy(buf, prefix);
    vsnprintf(buf + sizeof(prefix) - 1, sizeof(buf) - sizeof(prefix) + 1, fmt, args);
    messages.emplace_back(buf);

    return LIBSPECTRUM_ERROR_NONE;
}
