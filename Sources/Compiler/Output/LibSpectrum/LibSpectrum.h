#ifndef COMPILER_OUTPUT_LIBSPECTRUM_LIBSPECTRUM_H
#define COMPILER_OUTPUT_LIBSPECTRUM_LIBSPECTRUM_H

#include "Common/Common.h"

extern "C" {
#include <libspectrum.h>
#include <internals.h> // libspectrum
}

class LibSpectrum : private std::lock_guard<std::mutex>
{
public:
    LibSpectrum();
    ~LibSpectrum();

    const std::vector<std::string>& errorMessages() { return messages; }

    void throwIfError() const;

private:
    static std::mutex mutex;
    static std::vector<std::string> messages;

    static libspectrum_error errorFn(libspectrum_error, const char* fmt, va_list args);

    DISABLE_COPY(LibSpectrum);
};

#endif
