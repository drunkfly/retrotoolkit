#include "SpectrumTapeWriter.h"
#include "Common/StreamUtils.h"
#include "Compiler/Output/LibSpectrum/LibSpectrumTape.h"

#ifdef max
#undef max
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SpectrumTapeWriter::DiskFile
{
public:
    explicit DiskFile(uint8_t type) : mType(type) {}
    virtual ~DiskFile() {}

    size_t dataSize() const { return mData.size(); }

    DiskFile& setName(std::string name)
    {
        mName = std::move(name);
        if (mName.length() < 10)
            mName.resize(10, ' ');
        return *this;
    }

    DiskFile& appendByte(uint8_t data) { mData.emplace_back(data); return *this; }

    DiskFile& appendData(const void* ptr, size_t size)
    {
        const char* p = reinterpret_cast<const char*>(ptr);
        mData.insert(mData.end(), p, p + size);
        return *this;
    }

    void writeHeader(LibSpectrumTape& tape) const
    {
        std::stringstream ss;
        writeByte(ss, mType);
        ss.write(mName.data(), 10);
        writeParams(ss);
        tape.appendBlockString(ss.str(), 0);
    }

    void writeData(LibSpectrumTape& tape) const
    {
        tape.appendBlockRaw(mData.data(), mData.size(), 255, 100);
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

class SpectrumTapeWriter::BasicFile : public DiskFile
{
public:
    BasicFile()
        : DiskFile(0x00)
        , mInMemorySize(0)
        , mAutoStartLine(32768) // no autostart
    {
    }

    void setInMemorySize(size_t size) { mInMemorySize = size; }
    void setAutoStartLine(int line) { mAutoStartLine = line; }

protected:
    void writeParams(std::stringstream& ss) const override
    {
        size_t onTapeSize = dataSize();
        size_t inMemorySize = std::max(onTapeSize, mInMemorySize);
        writeWordLE(ss, uint16_t(inMemorySize));
        writeWordLE(ss, uint16_t(int16_t(mAutoStartLine)));
        writeWordLE(ss, uint16_t(onTapeSize));
    }

private:
    size_t mInMemorySize;
    int mAutoStartLine;

    DISABLE_COPY(BasicFile);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SpectrumTapeWriter::CodeFile : public DiskFile
{
public:
    CodeFile()
        : DiskFile(0x03)
        , mStartAddress(0)
    {
    }

    void setStartAddress(unsigned size) { mStartAddress = size; }

protected:
    void writeParams(std::stringstream& ss) const override
    {
        writeWordLE(ss, uint16_t(dataSize()));
        writeWordLE(ss, uint16_t(mStartAddress));
        ss << '\x00';
        ss << '\x80';
    }

private:
    unsigned mStartAddress;

    DISABLE_COPY(CodeFile);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SpectrumTapeWriter::SpectrumTapeWriter()
{
}

SpectrumTapeWriter::~SpectrumTapeWriter()
{
}

void SpectrumTapeWriter::addBasicFile(std::string name, const std::string& data, int startLine)
{
    auto basicFile = std::make_unique<BasicFile>();
    basicFile->setName(std::move(name));
    if (startLine >= 0)
        basicFile->setAutoStartLine(startLine);
    basicFile->appendData(data.c_str(), data.length());
    mFiles.emplace_back(std::move(basicFile));
}

void SpectrumTapeWriter::addCodeFile(std::string name, const CodeEmitter::Byte* data, size_t size, size_t startAddress)
{
    auto codeFile = std::make_unique<CodeFile>();
    codeFile->setName(std::move(name));
    codeFile->setStartAddress(startAddress);
    for (size_t i = 0; i < size; i++)
        codeFile->appendByte(data[i].value);
    mFiles.emplace_back(std::move(codeFile));
}

void SpectrumTapeWriter::setWriteTapFile(std::filesystem::path path)
{
    mTapFile = std::move(path);
}

void SpectrumTapeWriter::setWriteWavFile(std::filesystem::path path)
{
    mWavFile = std::move(path);
}

void SpectrumTapeWriter::writeOutput()
{
    LibSpectrum lib;
    LibSpectrumTape tape(lib);

    for (const auto& file : mFiles) {
        file->writeHeader(tape);
        file->writeData(tape);
    }

    if (mTapFile)
        tape.writeFile(LIBSPECTRUM_ID_TAPE_TAP, *mTapFile);

    if (mWavFile)
        tape.writeWavFile(*mWavFile);

    lib.throwIfError();
}
