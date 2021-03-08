#include "TRDOSWriter.h"
#include "Common/Strings.h"
#include "Common/StreamUtils.h"
#include "Common/IO.h"
#include <assert.h>
#include <string.h>
#include <sstream>

enum
{
    SectorSize = 256,
    MaxFiles = 128,
    SectorsPerTrack = 16,
    TotalSectors = (80 * 2 * SectorsPerTrack),
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TRDOSWriter::DiskFile
{
public:
    explicit DiskFile(uint8_t type) : mType(type) {}
    virtual ~DiskFile() {}

    size_t dataSize() const { return mData.size(); }
    size_t sizeInSectors() const { return (dataSize() + SectorSize - 1) / SectorSize; }

    DiskFile& setName(std::string name)
    {
        mName = std::move(name);
        if (mName.length() < 8)
            mName.resize(8, ' ');
        return *this;
    }

    DiskFile& appendByte(uint8_t data) { mData.emplace_back(data); return *this; }

    DiskFile& appendData(const void* ptr, size_t size)
    {
        const char* p = reinterpret_cast<const char*>(ptr);
        mData.insert(mData.end(), p, p + size);
        return *this;
    }

    virtual void finalizeData() {}

    void writeHeader(std::stringstream& ss) const
    {
        ss.write(mName.data(), 8);
        writeByte(ss, mType);
        writeParams(ss);
        writeByte(ss, sizeInSectors());
    }

    void writeData(std::stringstream& ss)
    {
        size_t dataSize = mData.size();
        size_t fullSize = sizeInSectors() * SectorSize;

        std::unique_ptr<char[]> buf{new char[fullSize]};
        memcpy(buf.get(), mData.data(), dataSize);
        memset(buf.get() + dataSize, 0, fullSize - dataSize);

        ss.write(buf.get(), fullSize);
    }

protected:
    virtual void writeParams(std::stringstream& ss) const = 0;

private:
    std::string mName;
    std::vector<char> mData;
    uint8_t mType;

    DISABLE_COPY(DiskFile);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TRDOSWriter::BasicFile : public DiskFile
{
public:
    BasicFile()
        : DiskFile(0x42) // 'B'
        , mInMemorySize(0)
        , mAutoStartLine(32768) // no autostart
    {
    }

    void setInMemorySize(size_t size) { mInMemorySize = size; }
    void setAutoStartLine(int line) { mAutoStartLine = line; }

    void finalizeData() override
    {
        appendByte(uint8_t(128));
        appendByte(uint8_t(170));
        appendByte(uint8_t(int16_t(mAutoStartLine) & 0xff));
        appendByte(uint8_t(int16_t(mAutoStartLine) >> 8));
        DiskFile::finalizeData();
    }

protected:
    void writeParams(std::stringstream& ss) const override
    {
        writeWordLE(ss, uint16_t(std::max(dataSize(), mInMemorySize)));
        writeWordLE(ss, uint16_t(dataSize()));
    }

private:
    size_t mInMemorySize;
    int mAutoStartLine;

    DISABLE_COPY(BasicFile);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TRDOSWriter::CodeFile : public DiskFile
{
public:
    CodeFile()
        : DiskFile(0x43) // 'C'
        , mStartAddress(0)
    {
    }

    void setStartAddress(unsigned size) { mStartAddress = size; }

protected:
    void writeParams(std::stringstream& ss) const override
    {
        writeWordLE(ss, uint16_t(mStartAddress));
        writeWordLE(ss, uint16_t(dataSize()));
    }

private:
    unsigned mStartAddress;

    DISABLE_COPY(CodeFile);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TRDOSWriter::TRDOSWriter()
{
}

TRDOSWriter::~TRDOSWriter()
{
}

void TRDOSWriter::addBasicFile(std::string name, const std::string& data, int startLine)
{
    auto basicFile = std::make_unique<BasicFile>();
    basicFile->setName(std::move(name));
    if (startLine >= 0)
        basicFile->setAutoStartLine(startLine);
    basicFile->appendData(data.c_str(), data.length());
    basicFile->finalizeData();
    mFiles.emplace_back(std::move(basicFile));
}

void TRDOSWriter::addCodeFile(std::string name, const CodeEmitter::Byte* data, size_t size, size_t startAddress)
{
    auto codeFile = std::make_unique<CodeFile>();
    codeFile->setName(std::move(name));
    codeFile->setStartAddress(startAddress);
    for (size_t i = 0; i < size; i++)
        codeFile->appendByte(data[i].value);
    codeFile->finalizeData();
    mFiles.emplace_back(std::move(codeFile));
}

void TRDOSWriter::writeSclFile(const std::filesystem::path& path)
{
    std::stringstream ss;
    ss.write("SINCLAIR", 8);
    writeByte(ss, uint8_t(mFiles.size()));
    for (const auto& file : mFiles)
        file->writeHeader(ss);
    for (const auto& file : mFiles)
        file->writeData(ss);

    std::string data = ss.str();
    return writeFile(path, data);
}

void TRDOSWriter::writeTrdFile(const std::filesystem::path& path, std::string volumeName)
{
    if (volumeName.length() < 8)
        volumeName.resize(8, ' ');

    std::stringstream ss;

    // sectors 0..7
    size_t sector = SectorsPerTrack;
    for (size_t i = 0; i < MaxFiles; i++) {
        if (i >= mFiles.size())
            ss.write("\1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16);
        else {
            mFiles[i]->writeHeader(ss);
            writeByte(ss, uint8_t(sector % SectorsPerTrack));
            writeByte(ss, uint8_t(sector / SectorsPerTrack));
            sector += mFiles[i]->sizeInSectors();
        }
    }

    // sector 8
    char buf[SectorSize] = {0};
    ss.write(buf, 225);
    writeByte(ss, uint8_t(sector % SectorsPerTrack));   // 225: next free sector
    writeByte(ss, uint8_t(sector / SectorsPerTrack));   // 226: next free track
    writeByte(ss, uint8_t(0x16));                       // 227: disk type (80 track, 2 side)
    writeByte(ss, uint8_t(mFiles.size()));              // 228: file count
    writeWordLE(ss, uint16_t(TotalSectors - sector));   // 229: free sectors count
    writeByte(ss, uint8_t(SectorsPerTrack));            // 231: sectors per track
    ss.write("\0\0         ", 12);                      // 232: reserved
    writeByte(ss, uint8_t(0));                          // 244: deleted files count
    ss.write(volumeName.c_str(), 8);                    // 245: volume name
    ss.write("\0\0\0", 3);                              // 253: reserved

    // sector 9..15
    memset(buf, 0, sizeof(buf));
    for (size_t i = 9; i < 16; i++)
        ss.write(buf, 256);

    // sectors 16... (with data)
    for (const auto& file : mFiles)
        file->writeData(ss);

    // remaining empty sectors
    for (size_t i = sector; i < TotalSectors; i++)
        ss.write(buf, SectorSize);

    std::string data = ss.str();
    assert(data.size() == TotalSectors * SectorSize);
    writeFile(path, data);
}
