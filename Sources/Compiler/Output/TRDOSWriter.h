#ifndef COMPILER_OUTPUT_TRDOSWRITER_H
#define COMPILER_OUTPUT_TRDOSWRITER_H

#include "Compiler/Output/IOutputWriter.h"

class TRDOSWriter final : public IOutputWriter
{
public:
    TRDOSWriter();
    ~TRDOSWriter();

    void addBasicFile(SourceLocation* location, std::string name,
        const std::string& data, int startLine = -1) override;

    void addCodeFile(SourceLocation* location, std::string name,
        const std::string& originalName, const CodeEmitter::Byte* data, size_t size, size_t startAddress) override;

    void setWriteSclFile(std::filesystem::path path);
    void setWriteTrdFile(std::filesystem::path path, std::string volumeName);

    void writeOutput() override;

private:
    class DiskFile;
    class BasicFile;
    class CodeFile;

    std::vector<std::unique_ptr<DiskFile>> mFiles;
    std::optional<std::filesystem::path> mSclFile;
    std::optional<std::filesystem::path> mTrdFile;
    std::optional<std::string> mVolumeName;

    void writeSclFile(const std::filesystem::path& path);
    void writeTrdFile(const std::filesystem::path& path, std::string volumeName);

    DISABLE_COPY(TRDOSWriter);
};

#endif
