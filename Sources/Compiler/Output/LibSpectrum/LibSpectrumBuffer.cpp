#include "LibSpectrumBuffer.h"

LibSpectrumBuffer::~LibSpectrumBuffer()
{
    if (mBuffer)
        libspectrum_free(mBuffer);
}
