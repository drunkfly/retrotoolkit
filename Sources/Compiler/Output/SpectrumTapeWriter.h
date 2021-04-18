#ifndef COMPILER_OUTPUT_SPECTRUMTAPEWRITER_H
#define COMPILER_OUTPUT_SPECTRUMTAPEWRITER_H

#include "Compiler/Output/IOutputWriter.h"

class SpectrumTapeWriter final : public IOutputWriter
{
public:
    SpectrumTapeWriter();
    ~SpectrumTapeWriter();

    void addBasicFile(SourceLocation* location, std::string name,
        const std::string& data, int startLine = -1) override;

    void addCodeFile(SourceLocation* location, std::string name,
        const std::string& originalName, const CodeEmitter::Byte* data, size_t size, size_t startAddress) override;

    void setWriteTapFile(std::filesystem::path path);
    void setWriteWavFile(std::filesystem::path path);

    void writeOutput() override;

private:
    class DiskFile;
    class BasicFile;
    class CodeFile;

    std::vector<std::unique_ptr<DiskFile>> mFiles;
    std::optional<std::filesystem::path> mTapFile;
    std::optional<std::filesystem::path> mWavFile;

    DISABLE_COPY(SpectrumTapeWriter);
};

#endif
