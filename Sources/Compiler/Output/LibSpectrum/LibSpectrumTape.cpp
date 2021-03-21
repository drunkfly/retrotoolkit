#include "LibSpectrumTape.h"
#include "Common/IO.h"
#include "Compiler/Output/LibSpectrum/VirtualFile.h"
#include "Compiler/Output/LibSpectrum/LibSpectrumTapeBlock.h"
#include "Compiler/Output/LibSpectrum/LibSpectrumBuffer.h"
#include "Compiler/CompilerError.h"

extern "C" {
//int write_tape(AFvirtualfile* vf, libspectrum_tape* tape, int sample_rate); // tape2wav
}

LibSpectrumTape::LibSpectrumTape(LibSpectrum& lib)
    : mLibSpectrum(lib)
{
    mTape = libspectrum_tape_alloc();
    if (!mTape) {
        mLibSpectrum.throwIfError();
        throw CompilerError(nullptr, "Unable to initialize tape writer.");
    }
}

LibSpectrumTape::~LibSpectrumTape()
{
    libspectrum_tape_free(mTape);
}

void LibSpectrumTape::appendBlock(LibSpectrumTapeBlock& block)
{
    libspectrum_tape_append_block(mTape, block);
    block.mBlock = nullptr;
    mLibSpectrum.throwIfError();
}

void LibSpectrumTape::appendBlockRaw(const void* data, size_t length, uint8_t flag, int pauseMs)
{
    LibSpectrumTapeBlock block(mLibSpectrum, LIBSPECTRUM_TAPE_BLOCK_ROM);
    block.setPause(pauseMs);
    block.setDataWithChecksum(data, length, flag);
    appendBlock(block);
}

void LibSpectrumTape::appendBlockString(const std::string& data, uint8_t flag, int pauseMs)
{
    appendBlockRaw(data.data(), data.length(), flag, pauseMs);
}

void LibSpectrumTape::write(LibSpectrumBuffer& buffer, size_t* length, libspectrum_id_t type)
{
    auto error = libspectrum_tape_write(buffer, length, mTape, type);
    mLibSpectrum.throwIfError();
    if (error != LIBSPECTRUM_ERROR_NONE)
        throw CompilerError(nullptr, "Unable to write tape data.");
}

void LibSpectrumTape::writeFile(libspectrum_id_t type, const std::filesystem::path& path)
{
    size_t length = 0;
    LibSpectrumBuffer buffer;
    write(buffer, &length, type);
    mLibSpectrum.throwIfError();
    ::writeFile(path, buffer.constData(), length, 0);
}

void LibSpectrumTape::writeWavFile(const std::filesystem::path& path)
{
    VirtualFile vf;
    //auto result1 = write_tape(vf, mTape, 44100);
    mLibSpectrum.throwIfError();
    //if (result1 != 0)
    //    throw CompilerError(nullptr, "Unable to write tape data.");

    ::writeFile(path, vf.data(), vf.length(), 0);
}
