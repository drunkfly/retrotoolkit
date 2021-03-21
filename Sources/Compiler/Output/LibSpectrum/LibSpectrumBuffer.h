#ifndef COMPILER_OUTPUT_LIBSPECTRUM_LIBSPECTRUMBUFFER_H
#define COMPILER_OUTPUT_LIBSPECTRUM_LIBSPECTRUMBUFFER_H

#include "Common/Common.h"
#include "Compiler/Output/LibSpectrum/LibSpectrum.h"

class LibSpectrumBuffer
{
public:
    LibSpectrumBuffer() : mBuffer(nullptr) {}
    ~LibSpectrumBuffer();

    const char* constData() const { return reinterpret_cast<const char*>(mBuffer); }

    operator libspectrum_byte**() { return &mBuffer; }

private:
    libspectrum_byte* mBuffer;

    DISABLE_COPY(LibSpectrumBuffer);
};

#endif
