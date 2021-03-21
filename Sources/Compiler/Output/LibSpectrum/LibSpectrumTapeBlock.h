#ifndef COMPILER_OUTPUT_LIBSPECTRUM_LIBSPECTRUMTAPEBLOCK_H
#define COMPILER_OUTPUT_LIBSPECTRUM_LIBSPECTRUMTAPEBLOCK_H

#include "Common/Common.h"
#include "Compiler/Output/LibSpectrum/LibSpectrum.h"

class LibSpectrum;
class LibSpectrumTape;

class LibSpectrumTapeBlock
{
public:
    LibSpectrumTapeBlock(LibSpectrum& lib, libspectrum_tape_type type);
    ~LibSpectrumTapeBlock();

    void setPause(int ms);
    void setDataWithChecksum(const void* data, size_t length, uint8_t flag = 0);

    operator libspectrum_tape_block*() const { return mBlock; }

private:
    LibSpectrum& mLibSpectrum;
    libspectrum_tape_block* mBlock;

    DISABLE_COPY(LibSpectrumTapeBlock);
    friend class LibSpectrumTape;
};

#endif
