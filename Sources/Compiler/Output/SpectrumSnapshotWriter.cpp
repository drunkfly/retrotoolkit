#include "SpectrumSnapshotWriter.h"
#include "Compiler/LexerUtils.h"
#include "Compiler/CompilerError.h"
#include "Common/Strings.h"
#include "Common/StreamUtils.h"
#include "Common/IO.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SpectrumSnapshotWriter::SpectrumSnapshotWriter()
    : mZ80Format(Z80Format::Auto)
    , mZ80Machine(Z80Machine::Spectrum128k)
    , mA(0)
    , mF(0)
    , mBC(0)
    , mHL(0)
    , mDE(0)
    , mShadowA(0)
    , mShadowF(0)
    , mShadowBC(0)
    , mShadowHL(0)
    , mShadowDE(0)
    , mPC(0x8000)
    , mSP(0x8000)
    , mIY(0)
    , mIX(0)
    , mI(0)
    , mR(0)
    , mBorderColor(0)
    , mInterruptMode(0)
    , mPort7FFD(0)
    , mPort1FFD(0xff)
    , mPortFFFD(0)
{
    memset(mSoundChipRegisters, 0, sizeof(mSoundChipRegisters));
}

SpectrumSnapshotWriter::~SpectrumSnapshotWriter()
{
}

void SpectrumSnapshotWriter::addBasicFile(SourceLocation* location, std::string, const std::string&, int)
{
    throw CompilerError(location, "snapshot writer does not support BASIC code.");
}

void SpectrumSnapshotWriter::addCodeFile(SourceLocation* location, std::string name,
    const std::string& originalName, const CodeEmitter::Byte* data, size_t size, size_t startAddress)
{
    if (startAddress < 0x4000)
        throw CompilerError(location, "starting address should be >= 0x4000.");

    int bank = 0;
    if (startsWith(toUpper(name), "BANK") && name.length() > 4 && isDigit(name[4])) {
        bank = charToInt(name[4]);
        if (bank < 0 || bank > 7 || name.length() != 5)
            throw CompilerError(location, "invalid bank number (should be in range 0 to 7).");

        if (mZ80Format == Z80Format::Auto)
            mZ80Format = Z80Format::Version2;
        else if (mZ80Format == Z80Format::Version1)
            throw CompilerError(location, "version 1 of Z80 format does not support memory banks.");
    }

    if (size == 0)
        return;

    for (const auto& it : mFiles) {
        if ((startAddress + size - 1) >= it.start && startAddress <= (it.start + it.size - 1)) {
            if (startAddress < 0xc000 && it.start < 0xc000) {
                std::stringstream ss;
                ss << "File \"" << originalName << "\" overlaps with file \"" << it.name << "\".";
                throw CompilerError(location, ss.str());
            } else if (it.bank == bank) {
                std::stringstream ss;
                ss << "File \"" << originalName << "\" overlaps with file \"" << it.name << "\" in bank " << bank << '.';
                throw CompilerError(location, ss.str());
            }
        }
    }

    File file;
    file.bank = bank;
    file.start = startAddress;
    file.size = size;
    file.name = originalName;
    file.bytes.reset(new uint8_t[size]);
    for (size_t i = 0; i < size; i++)
        file.bytes[i] = data[i].value;
    mFiles.emplace_back(std::move(file));
}

void SpectrumSnapshotWriter::setWriteZ80File(std::filesystem::path path, Z80Format format)
{
    mZ80File = std::move(path);
    mZ80Format = format;
}

void SpectrumSnapshotWriter::writeOutput()
{
    if (mZ80File)
        writeZ80File(*mZ80File);
}

void SpectrumSnapshotWriter::writeZ80File(const std::filesystem::path& path)
{
    std::stringstream ss;

    if (mZ80Format == Z80Format::Auto) {
        mZ80Format = Z80Format::Version1;
        for (const auto& it : mFiles) {
            if (it.start + it.size > 0xc000) {
                mZ80Format = Z80Format::Version2;
                break;
            }
        }
    }

    writeByte(ss, mA);                                                  // 0
    writeByte(ss, mF);                                                  // 1
    writeWordLE(ss, mBC);                                               // 2
    writeWordLE(ss, mHL);                                               // 4
    writeWordLE(ss, (mZ80Format == Z80Format::Version1 ? mPC : 0));     // 6
    writeWordLE(ss, mSP);                                               // 8
    writeByte(ss, mI);                                                  // 10
    writeByte(ss, mR);                                                  // 11

    uint8_t flags1 = 0;
    flags1 |= (mBorderColor & 7) << 1;
    if ((mR & 0x80) != 0)
        flags1 |= 0x01;
    writeByte(ss, flags1);                                              // 12

    writeWordLE(ss, mDE);                                               // 13
    writeWordLE(ss, mShadowBC);                                         // 15
    writeWordLE(ss, mShadowDE);                                         // 17
    writeWordLE(ss, mShadowHL);                                         // 19
    writeByte(ss, mShadowA);                                            // 21
    writeByte(ss, mShadowF);                                            // 22
    writeWordLE(ss, mIY);                                               // 23
    writeWordLE(ss, mIX);                                               // 25
    writeByte(ss, 1); // interrupts enabled                             // 27
    writeByte(ss, 0); // IFF2                                           // 28

    uint8_t flags2 = 0;
    flags2 |= (mInterruptMode <= 2 ? mInterruptMode : 0);
    flags2 |= 0xc0; // kempston
    writeByte(ss, flags2);                                              // 29

    switch (mZ80Format) {
        case Z80Format::Version1: {
            std::unique_ptr<uint8_t[]> memory{new uint8_t[49152]};
            for (const auto& it : mFiles) {
                assert(it.start >= 0x4000);
                assert(it.start + it.size <= 0xffff);
                memcpy(&memory[it.start - 0x4000], it.bytes.get(), it.size);
            }
            ss.write(reinterpret_cast<char*>(memory.get()), 49152);
            break;
        }

        case Z80Format::Version2:
        case Z80Format::Version3: {
            writeWordLE(ss, (mZ80Format == Z80Format::Version2 ? 23 : (mPort1FFD != 0xff ? 55 : 54)));  // 30
            writeWordLE(ss, mPC);                                                                       // 32
            switch (mZ80Machine) {
                case Z80Machine::Spectrum16k: writeByte(ss, 0); break;                                  // 34
                case Z80Machine::Spectrum48k: writeByte(ss, 0); break;
                case Z80Machine::Spectrum128k: writeByte(ss, (mZ80Format == Z80Format::Version2 ? 3 : 4)); break;
                case Z80Machine::SpectrumPlus3: writeByte(ss, 7); break;
                case Z80Machine::Pentagon: writeByte(ss, 9); break;
                case Z80Machine::Scorpion: writeByte(ss, 10); break;
                case Z80Machine::DidaktikKompakt: writeByte(ss, 11); break;
                case Z80Machine::SpectrumPlus2: writeByte(ss, 12); break;
                case Z80Machine::SpectrumPlus2A: writeByte(ss, 13); break;
                case Z80Machine::TC2048: writeByte(ss, 14); break;
                case Z80Machine::TC2068: writeByte(ss, 15); break;
                case Z80Machine::TS2068: writeByte(ss, 128); break;
            }
            writeByte(ss, mPort7FFD); // FIXME: timex: last OUT to 0xF4                                 // 35
            writeByte(ss, 0);         // FIXME: timex: last OUT to 0xFF                                 // 36

            uint8_t flags3 = 0;
            if (mZ80Machine == Z80Machine::Spectrum16k)
                flags3 |= 0x80;
            writeByte(ss, flags3);                                                                      // 37
            writeByte(ss, mPortFFFD);                                                                   // 38
            ss.write(reinterpret_cast<const char*>(mSoundChipRegisters), 16);                           // 39

            if (mZ80Format == Z80Format::Version3) {
                writeWordLE(ss, 0);     // low T state counter                                          // 55
                writeByte(ss, 0);       // high T state counter                                         // 57
                writeByte(ss, 0);       // ignored by Z80                                               // 58
                writeByte(ss, 0);       // MGT ROM paged?                                               // 59
                writeByte(ss, 0);       // Multiface ROM paged?                                         // 60
                writeByte(ss, 0xff);    // First 8k is ROM?                                             // 61
                writeByte(ss, 0xff);    // Second 8k is ROM?                                            // 62
                for (size_t i = 0; i < 20; i++) // joystick-to-keyboard mappings                        // 63
                    writeByte(ss, 0);
                writeByte(ss, 0);       // MGT type                                                     // 83
                writeByte(ss, 0);       // Disciple inhibit button status                               // 84
                writeByte(ss, 0);       // Disciple inhibit flag                                        // 85
                if (mPort1FFD != 0xff)
                    writeByte(ss, mPort1FFD);                                                           // 86
            }

            break;
        }

        default:
            assert(false);
            throw CompilerError(nullptr, "internal compiler error: unsupported z80 format.");
    }

    std::string data = ss.str();
    return writeFile(path, data);
}
