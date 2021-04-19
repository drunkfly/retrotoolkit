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
    Auto,
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

    void setFormat(Z80Format format) { mZ80Format = format; }
    void setMachine(Z80Machine machine) { mZ80Machine = machine; }

    void setA(uint8_t a) { mA = a; }
    void setF(uint8_t f) { mF = f; }
    void setBC(uint16_t bc) { mBC = bc; }
    void setHL(uint16_t hl) { mHL = hl; }
    void setDE(uint16_t de) { mDE = de; }
    void setShadowA(uint8_t a) { mShadowA = a; }
    void setShadowF(uint8_t f) { mShadowF = f; }
    void setShadowBC(uint16_t bc) { mShadowBC = bc; }
    void setShadowHL(uint16_t hl) { mShadowHL = hl; }
    void setShadowDE(uint16_t de) { mShadowDE = de; }
    void setPC(uint16_t pc) { mPC = pc; }
    void setSP(uint16_t sp) { mSP = sp; }
    void setIY(uint16_t iy) { mIY = iy; }
    void setIX(uint16_t ix) { mIX = ix; }
    void setI(uint8_t i) { mI = i; }
    void setR(uint8_t r) { mR = r; }

    void setPort7FFD(uint8_t v) { mPort7FFD = v; }
    void setPortFFFD(uint8_t v) { mPort7FFD = v; }
    void setPort1FFD(uint8_t v) { mPort1FFD = v; }

    void setBorderColor(uint8_t c) { mBorderColor = c; }
    void setInterruptMode(uint8_t im) { mInterruptMode = im; }
    void setInterruptsEnabled(bool flag) { mInterruptsEnabled = flag; }

    void addBasicFile(SourceLocation* location, std::string name,
        const std::string& data, int startLine = -1) override;

    void addCodeFile(SourceLocation* location, std::string name,
        const std::string& originalName, const CodeEmitter::Byte* data, size_t size, size_t startAddress) override;

    void setWriteZ80File(SourceLocation* loc, std::filesystem::path path, Z80Format format = Z80Format::Auto);

    void writeOutput() override;

private:
    struct File
    {
        int bank;
        size_t start;
        size_t size;
        SourceLocation* location;
        std::string name;
        std::unique_ptr<uint8_t[]> bytes;
    };

    std::vector<File> mFiles;
    std::optional<std::filesystem::path> mZ80File;
    SourceLocation* mZ80Location;
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
    uint8_t mI;
    uint8_t mR;
    uint8_t mBorderColor;
    uint8_t mInterruptMode;
    uint8_t mPort7FFD;
    uint8_t mPortFFFD;
    uint16_t mPort1FFD;
    uint8_t mSoundChipRegisters[16];
    bool mInterruptsEnabled;

    void writeZ80File(const std::filesystem::path& path);

    std::unique_ptr<uint8_t[]> buildMemory();

    DISABLE_COPY(SpectrumSnapshotWriter);
};

#endif
