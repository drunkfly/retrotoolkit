#include "SpectrumSnapshotWriter.h"
#include "Compiler/LexerUtils.h"
#include "Compiler/CompilerError.h"
#include "Common/Strings.h"
#include "Common/StreamUtils.h"
#include "Common/IO.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SpectrumSnapshotWriter::SpectrumSnapshotWriter()
    : mZ80Location(nullptr)
    , mZ80Format(Z80Format::Auto)
    , mZ80Machine(Z80Machine::Auto)
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
    , mBorderColor(7)
    , mInterruptMode(0)
    , mPort7FFD(0)
    , mPort1FFD(0xffff)
    , mPortFFFD(0)
    , mInterruptsEnabled(true)
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
    int bank = 0;
    bool hasBank = false;
    if (startsWith(toUpper(name), "BANK") && name.length() > 4 && isDigit(name[4])) {
        hasBank = true;

        bank = charToInt(name[4]);
        if (bank < 0 || bank > 7 || name.length() != 5)
            throw CompilerError(location, "invalid bank number (should be in range 0 to 7).");

        if (mZ80Format == Z80Format::Auto)
            mZ80Format = Z80Format::Version2;
        else if (bank != 0 && bank != 2 && bank != 5 && size != 0 && mZ80Format == Z80Format::Version1)
            throw CompilerError(location, "version 1 of Z80 format does not support memory banks.");
    }

    if (size == 0)
        return;

    if (!hasBank) {
        if (size > 3 * 16384)
            throw CompilerError(location, "file is greater than 48K.");
        if (startAddress < 0x4000 || startAddress + size > 0x10000)
            throw CompilerError(location, "file does not fit memory bounds (0x4000..0xffff).");

        size_t start1 = startAddress;
        size_t end1 = start1 + size;
        for (const auto& it : mFiles) {
            size_t start2;
            switch (it.bank) {
                case 0: start2 = it.start; break;
                case 2: start2 = it.start - (0xc000 - 0x8000); break;
                case 5: start2 = it.start - (0xc000 - 0x4000); break;
                default: continue;
            }

            size_t end2 = (start2 + it.size - 1);
            if (end1 >= start2 && start1 <= end2) {
                std::stringstream ss;
                ss << "File \"" << originalName << "\" overlaps with file \"" << it.name << "\".";
                throw CompilerError(location, ss.str());
            }
        }

        size_t off1 = 0;

        if (start1 < 0x8000) {
            File file;
            file.bank = 5;
            file.start = start1;
            file.size = std::min<size_t>(end1, 0x8000) - file.start;
            file.location = location;
            file.name = originalName;
            file.bytes.reset(new uint8_t[file.size]);
            for (size_t i = 0; i < file.size; i++)
                file.bytes[i] = data[i].value;
            file.start += (0xc000 - 0x4000);
            mFiles.emplace_back(std::move(file));
            off1 += file.size;
        }

        if (end1 > 0x8000 && start1 < 0xc000) {
            File file;
            file.bank = 2;
            file.start = std::max<size_t>(start1, 0x8000);
            file.size = std::min<size_t>(end1, 0xc000) - file.start;
            file.location = location;
            file.name = originalName;
            file.bytes.reset(new uint8_t[file.size]);
            for (size_t i = 0; i < file.size; i++)
                file.bytes[i] = data[off1 + i].value;
            file.start += (0xc000 - 0x8000);
            mFiles.emplace_back(std::move(file));
            off1 += file.size;
        }

        if (end1 > 0xc000) {
            File file;
            file.bank = 0;
            file.start = std::max<size_t>(start1, 0xc000);
            file.size = std::min<size_t>(end1, 0x10000) - file.start;
            file.location = location;
            file.name = originalName;
            file.bytes.reset(new uint8_t[file.size]);
            for (size_t i = 0; i < file.size; i++)
                file.bytes[i] = data[off1 + i].value;
            mFiles.emplace_back(std::move(file));
        }
    } else {
        if (size > 16384)
            throw CompilerError(location, "bank size should not exceed 16K.");

        if (bank == 2) {
            if (startAddress >= 0x8000 && startAddress + size <= 0xc000)
                startAddress += (0xc000 - 0x8000);
            else if (startAddress >= 0xc000 && startAddress + size <= 0x10000)
                ;
            else
                throw CompilerError(location, "file does not fit BANK 2 bounds (0x8000..0xbfff or 0xc000..0xffff).");
        } else if (bank == 5) {
            if (startAddress >= 0x4000 && startAddress + size <= 0x8000)
                startAddress += (0xc000 - 0x4000);
            else if (startAddress >= 0xc000 && startAddress + size <= 0x10000)
                ;
            else
                throw CompilerError(location, "file does not fit BANK 5 bounds (0x4000..0x7fff or 0xc000..0xffff).");
        } else {
            if (startAddress < 0xc000 || startAddress + size > 0x10000) {
                std::stringstream ss;
                ss << "file does not fit BANK " << bank << " bounds (0xc000..0xffff).";
                throw CompilerError(location, ss.str());
            }
        }

        for (const auto& it : mFiles) {
            if (it.bank != bank)
                continue;

            size_t start1 = startAddress;
            size_t start2 = it.start;
            size_t end1 = (start1 + size - 1);
            size_t end2 = (start2 + it.size - 1);
            if (end1 >= start2 && start1 <= end2) {
                std::stringstream ss;
                ss << "File \"" << originalName << "\" overlaps with file \"" << it.name << "\".";
                throw CompilerError(location, ss.str());
            }
        }

        File file;
        file.bank = bank;
        file.start = startAddress;
        file.size = size;
        file.location = location;
        file.name = originalName;
        file.bytes.reset(new uint8_t[size]);
        for (size_t i = 0; i < size; i++)
            file.bytes[i] = data[i].value;
        mFiles.emplace_back(std::move(file));
    }
}

void SpectrumSnapshotWriter::setWriteZ80File(SourceLocation* loc, std::filesystem::path path, Z80Format format)
{
    mZ80File = std::move(path);
    mZ80Format = format;
    mZ80Location = loc;
}

void SpectrumSnapshotWriter::addWriteExeFile(
    SourceLocation* loc, std::filesystem::path input, std::filesystem::path output)
{
    ExeFile file;
    file.location = loc;
    file.input = std::move(input);
    file.output = std::move(output);
    mExeFiles.emplace_back(std::move(file));
}

void SpectrumSnapshotWriter::writeOutput()
{
    if (mZ80File)
        writeZ80File(*mZ80File);

    for (const auto& it : mExeFiles)
        writeExeFile(it.location, it.input, it.output);
}

void SpectrumSnapshotWriter::writeZ80File(const std::filesystem::path& path)
{
    Z80Format format = mZ80Format;
    Z80Machine machine = mZ80Machine;
    std::stringstream ss;

    bool is48k = false;
    switch (machine) {
        case Z80Machine::Auto: break;
        case Z80Machine::Spectrum16k: is48k = true; break;
        case Z80Machine::Spectrum48k: is48k = true; break;
        case Z80Machine::Spectrum128k: break;
        case Z80Machine::SpectrumPlus3: break;
        case Z80Machine::Pentagon: break;
        case Z80Machine::Scorpion: break;
        case Z80Machine::DidaktikKompakt: is48k = true; break;
        case Z80Machine::SpectrumPlus2: break;
        case Z80Machine::SpectrumPlus2A: break;
        case Z80Machine::TC2048: is48k = true; break;
        case Z80Machine::TC2068: is48k = true; break;
        case Z80Machine::TS2068: is48k = true; break;
    }

    bool need128k = false;
    if (format == Z80Format::Auto) {
        format = Z80Format::Version1;
        if (mPort1FFD != 0xffff)
            format = Z80Format::Version3;
        else if (machine != Z80Machine::Spectrum48k && machine != Z80Machine::Auto)
            format = Z80Format::Version2;
        else if (mPC == 0)
            format = Z80Format::Version2;
        else {
            for (const auto& it : mFiles) {
                if (it.bank != 0 && it.bank != 2 && it.bank != 5) {
                    format = Z80Format::Version2;
                    need128k = true;
                    break;
                }
            }
        }
    }

    if (machine == Z80Machine::Auto) {
        if (mPort1FFD != 0xffff)
            machine = Z80Machine::SpectrumPlus3;
        else if (need128k)
            machine = Z80Machine::Spectrum128k;
        else {
            machine = Z80Machine::Spectrum48k;
            is48k = true;
        }
    }

    assert(format != Z80Format::Auto);
    assert(machine != Z80Machine::Auto);

    if (format == Z80Format::Version1 && mPC == 0)
        throw CompilerError(mZ80Location, "cannot write z80 format 1 with PC equal to 0.");
    if (format != Z80Format::Version3 && mPort1FFD != 0xffff)
        throw CompilerError(mZ80Location, "port 0x1ffd requires z80 format 3.");

    if (is48k) {
        for (const auto& it : mFiles) {
            if (it.bank != 0 && it.bank != 2 && it.bank != 5)
                throw CompilerError(it.location, "Invalid bank for 48k machine.");
        }
    }

    writeByte(ss, mA);                                                  // 0
    writeByte(ss, mF);                                                  // 1
    writeWordLE(ss, mBC);                                               // 2
    writeWordLE(ss, mHL);                                               // 4
    writeWordLE(ss, (format == Z80Format::Version1 ? mPC : 0));         // 6
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
    writeByte(ss, (mInterruptsEnabled ? 1 : 0));                        // 27
    writeByte(ss, 0); // IFF2                                           // 28

    uint8_t flags2 = 0;
    flags2 |= (mInterruptMode <= 2 ? mInterruptMode : 0);
    flags2 |= 0x40; // kempston
    writeByte(ss, flags2);                                              // 29

    switch (format) {
        case Z80Format::Version1:
            break;

        case Z80Format::Version2:
        case Z80Format::Version3: {
            writeWordLE(ss, (format == Z80Format::Version2 ? 23 : (mPort1FFD != 0xffff ? 55 : 54)));    // 30
            writeWordLE(ss, mPC);                                                                       // 32
            switch (machine) {
                case Z80Machine::Spectrum16k: writeByte(ss, 0); break;                                  // 34
                case Z80Machine::Spectrum48k: writeByte(ss, 0); break;
                case Z80Machine::Spectrum128k: writeByte(ss, (format == Z80Format::Version2 ? 3 : 4)); break;
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
            if (machine == Z80Machine::Spectrum16k)
                flags3 |= 0x80;
            writeByte(ss, flags3);                                                                      // 37
            writeByte(ss, mPortFFFD);                                                                   // 38
            ss.write(reinterpret_cast<const char*>(mSoundChipRegisters), 16);                           // 39

            if (format == Z80Format::Version3) {
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
                if (mPort1FFD != 0xffff)
                    writeByte(ss, uint8_t(mPort1FFD & 0xff));                                           // 86
            }

            break;
        }

        default:
            assert(false);
            throw CompilerError(mZ80Location, "internal compiler error: unsupported z80 format.");
    }

    std::unique_ptr<uint8_t[]> memory{new uint8_t[8 * 16384]};
    buildMemory(memory.get());

    if (format == Z80Format::Version1) {
        ss.write(reinterpret_cast<const char*>(&memory[5 * 16384]), 16384);
        ss.write(reinterpret_cast<const char*>(&memory[2 * 16384]), 16384);
        ss.write(reinterpret_cast<const char*>(&memory[0 * 16384]), 16384);
    } else {
        writeWordLE(ss, 0xffff);
        writeByte(ss, 8);               // bank 5: 0x4000 - 0x7fff
        ss.write(reinterpret_cast<const char*>(&memory[5 * 16384]), 16384);

        writeWordLE(ss, 0xffff);
        writeByte(ss, (is48k ? 4 : 5)); // bank 2: 0x8000 - 0xbfff
        ss.write(reinterpret_cast<const char*>(&memory[2 * 16384]), 16384);

        writeWordLE(ss, 0xffff);
        writeByte(ss, (is48k ? 5 : 3)); // bank 0: 0xc000 - 0xffff
        ss.write(reinterpret_cast<const char*>(&memory[0 * 16384]), 16384);

        if (!is48k) {
            writeWordLE(ss, 0xffff);
            writeByte(ss, 4);           // bank 1: 0xc000 - 0xffff
            ss.write(reinterpret_cast<const char*>(&memory[1 * 16384]), 16384);

            writeWordLE(ss, 0xffff);
            writeByte(ss, 6);           // bank 3: 0xc000 - 0xffff
            ss.write(reinterpret_cast<const char*>(&memory[3 * 16384]), 16384);

            writeWordLE(ss, 0xffff);
            writeByte(ss, 7);           // bank 4: 0xc000 - 0xffff
            ss.write(reinterpret_cast<const char*>(&memory[4 * 16384]), 16384);

            writeWordLE(ss, 0xffff);
            writeByte(ss, 9);           // bank 6: 0xc000 - 0xffff
            ss.write(reinterpret_cast<const char*>(&memory[6 * 16384]), 16384);

            writeWordLE(ss, 0xffff);
            writeByte(ss, 10);          // bank 7: 0xc000 - 0xffff
            ss.write(reinterpret_cast<const char*>(&memory[7 * 16384]), 16384);
        }
    }

    std::string data = ss.str();
    return writeFile(path, data);
}

void SpectrumSnapshotWriter::writeExeFile(SourceLocation* loc,
    const std::filesystem::path& input, const std::filesystem::path& output)
{
    std::string data = loadFile(input);

    size_t off = 0, len = data.size(), memOff = size_t(-1), regOff = size_t(-1);
    while (off <= len - 4) {
        if (memcmp(&data[off], "MEMORY\x1A\x1A", 8) == 0) {
            memOff = off;
            if (regOff != size_t(-1))
                break;
        }
        if (memcmp(&data[off], "REGISTERS\x1A\x1A", 11) == 0) {
            regOff = off;
            if (memOff != size_t(-1))
                break;
        }
        ++off;
    }

    if (regOff == size_t(-1) || memOff == size_t(-1) || regOff + 31 > len || memOff + 8 * 16384 > len) {
        std::stringstream ss;
        ss << "unable to embed memory data into file \"" << input << "\".";
        throw CompilerError(loc, ss.str());
    }

    buildMemory(&data[memOff]);

    data[regOff +  0] = mA;
    data[regOff +  1] = mF;
    data[regOff +  2] = (mBC >> 8) & 0xff;
    data[regOff +  3] = mBC & 0xff;
    data[regOff +  4] = (mDE >> 8) & 0xff;
    data[regOff +  5] = mDE & 0xff;
    data[regOff +  6] = mHL & 0xff;
    data[regOff +  7] = (mHL >> 8) & 0xff;
    data[regOff +  8] = mShadowA;
    data[regOff +  9] = mShadowF;
    data[regOff + 10] = (mShadowBC >> 8) & 0xff;
    data[regOff + 11] = mShadowBC & 0xff;
    data[regOff + 12] = (mShadowDE >> 8) & 0xff;
    data[regOff + 13] = mShadowDE & 0xff;
    data[regOff + 14] = mShadowHL & 0xff;
    data[regOff + 15] = (mShadowHL >> 8) & 0xff;
    data[regOff + 16] = (mIX >> 8) & 0xff;
    data[regOff + 17] = mIX & 0xff;
    data[regOff + 18] = (mIY >> 8) & 0xff;
    data[regOff + 19] = mIY & 0xff;
    data[regOff + 20] = mI;
    data[regOff + 21] = mR;
    data[regOff + 22] = mInterruptMode;
    data[regOff + 23] = (mInterruptsEnabled ? 0 : 1);
    data[regOff + 24] = mPC & 0xff;
    data[regOff + 25] = (mPC >> 8) & 0xff;
    data[regOff + 26] = mSP & 0xff;
    data[regOff + 27] = (mSP >> 8) & 0xff;
    data[regOff + 28] = (mPort1FFD == 0xffff ? 0 : uint8_t(mPort1FFD));
    data[regOff + 29] = mPort7FFD;
    data[regOff + 30] = mPortFFFD;

    writeFile(output, data);
}

void SpectrumSnapshotWriter::buildMemory(void* dst)
{
    auto p = reinterpret_cast<uint8_t*>(dst);
    memset(p, 0, 8 * 16384);

    for (const auto& it : mFiles) {
        assert(it.start >= 0xc000);
        assert(it.size <= 16384);
        assert(it.bank >= 0 && it.bank < 8);
        memcpy(&p[it.bank * 16384 + it.start - 0xc000], it.bytes.get(), it.size);
    }
}
