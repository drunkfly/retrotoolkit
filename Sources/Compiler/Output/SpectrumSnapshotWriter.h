#ifndef COMPILER_OUTPUT_SPECTRUMSNAPSHOTWRITER_H
#define COMPILER_OUTPUT_SPECTRUMSNAPSHOTWRITER_H

#include "Compiler/Output/IOutputWriter.h"

enum class Z80Format
{
    Auto,
    Version1,
    Version2,
    Version3,
};

enum class Z80Machine
{
    Spectrum16k,
    Spectrum48k,
    Spectrum128k,
    SpectrumPlus3,
    Pentagon,
    Scorpion,
    DidaktikKompakt,
    SpectrumPlus2,
    SpectrumPlus2A,
    TC2048,
    TC2068,
    TS2068,
};

class SpectrumSnapshotWriter final : public IOutputWriter
{
public:
    SpectrumSnapshotWriter();
    ~SpectrumSnapshotWriter();

    void addBasicFile(SourceLocation* location, std::string name,
        const std::string& data, int startLine = -1) override;

    void addCodeFile(SourceLocation* location, std::string name,
        const std::string& originalName, const CodeEmitter::Byte* data, size_t size, size_t startAddress) override;

    void setWriteZ80File(std::filesystem::path path, Z80Format format = Z80Format::Auto);

    void writeOutput() override;

private:
    struct File
    {
        int bank;
        size_t start;
        size_t size;
        std::string name;
        std::unique_ptr<uint8_t[]> bytes;
    };

    std::vector<File> mFiles;
    std::optional<std::filesystem::path> mZ80File;
    Z80Format mZ80Format;
    Z80Machine mZ80Machine;
    uint8_t mA;
    uint8_t mF;
    uint16_t mBC;
    uint16_t mHL;
    uint16_t mDE;
    uint8_t mShadowA;
    uint8_t mShadowF;
    uint16_t mShadowBC;
    uint16_t mShadowHL;
    uint16_t mShadowDE;
    uint16_t mPC;
    uint16_t mSP;
    uint16_t mIY;
    uint16_t mIX;
    uint16_t mI;
    uint16_t mR;
    uint8_t mBorderColor;
    uint8_t mInterruptMode;
    uint8_t mPort7FFD;
    uint8_t mPort1FFD;
    uint8_t mPortFFFD;
    uint8_t mSoundChipRegisters[16];

    void writeZ80File(const std::filesystem::path& path);

    DISABLE_COPY(SpectrumSnapshotWriter);
};

#endif
