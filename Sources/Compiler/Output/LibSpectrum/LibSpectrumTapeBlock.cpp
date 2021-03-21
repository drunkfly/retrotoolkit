#include "LibSpectrumTapeBlock.h"
#include "Compiler/CompilerError.h"

LibSpectrumTapeBlock::LibSpectrumTapeBlock(LibSpectrum& lib, libspectrum_tape_type type)
    : mLibSpectrum(lib)
{
    mBlock = libspectrum_tape_block_alloc(type);
    if (!mBlock) {
        mLibSpectrum.throwIfError();
        throw CompilerError(nullptr, "Unable to allocate tape block.");
    }
}

LibSpectrumTapeBlock::~LibSpectrumTapeBlock()
{
    if (mBlock)
        libspectrum_tape_block_free(mBlock);
}

void LibSpectrumTapeBlock::setPause(int ms)
{
    libspectrum_set_pause_ms(mBlock, ms);
    mLibSpectrum.throwIfError();
}

void LibSpectrumTapeBlock::setDataWithChecksum(const void* data, size_t length, uint8_t flag)
{
    auto error = libspectrum_tape_block_set_data_length(mBlock, length + 2);
    mLibSpectrum.throwIfError();
    if (error != LIBSPECTRUM_ERROR_NONE)
        throw CompilerError(nullptr, "Unable to set tape block data.");

    libspectrum_byte* dst = libspectrum_new(libspectrum_byte, length + 2);
    if (!dst) {
        mLibSpectrum.throwIfError();
        throw CompilerError(nullptr, "Unable to set tape block data.");
    }

    error = libspectrum_tape_block_set_data(mBlock, dst);
    if (error != LIBSPECTRUM_ERROR_NONE) {
        libspectrum_free(dst);
        mLibSpectrum.throwIfError();
        throw CompilerError(nullptr, "Unable to set tape block data.");
    }

    const char* src = reinterpret_cast<const char*>(data);
    const char* end = src + length;

    *dst++ = flag;
    libspectrum_byte checksum = flag;
    while (src < end) {
        libspectrum_byte byte = *src++;
        *dst++ = byte;
        checksum ^= byte;
    }
    *dst++ = checksum;
}
