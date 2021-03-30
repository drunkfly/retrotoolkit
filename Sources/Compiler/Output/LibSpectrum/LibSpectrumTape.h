#ifndef COMPILER_OUTPUT_LIBSPECTRUM_LIBSPECTRUMTAPE_H
#define COMPILER_OUTPUT_LIBSPECTRUM_LIBSPECTRUMTAPE_H

#include "Compiler/Output/LibSpectrum/LibSpectrum.h"

class LibSpectrum;
class LibSpectrumTapeBlock;
class LibSpectrumBuffer;

class LibSpectrumTape
{
public:
    explicit LibSpectrumTape(LibSpectrum& lib);
    ~LibSpectrumTape();

    void appendBlock(LibSpectrumTapeBlock& block);
    void appendBlockRaw(const void* data, size_t length, uint8_t flag = 0, int pauseMs = 1000);
    void appendBlockString(const std::string& data, uint8_t flag = 0, int pauseMs = 1000);

    void write(LibSpectrumBuffer& buffer, size_t* length, libspectrum_id_t type);
    void writeFile(libspectrum_id_t type, const std::filesystem::path& path);
    void writeWavFile(const std::filesystem::path& path);

    operator libspectrum_tape*() const { return mTape; }

private:
    LibSpectrum& mLibSpectrum;
    libspectrum_tape* mTape;

    DISABLE_COPY(LibSpectrumTape);
};

#endif
