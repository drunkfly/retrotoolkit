#ifndef COMPILER_OUTPUT_TRDOSWRITER_H
#define COMPILER_OUTPUT_TRDOSWRITER_H

#include "Common/Common.h"
#include "Compiler/Linker/CodeEmitter.h"
#include <string>
#include <memory>
#include <filesystem>

class TRDOSWriter
{
public:
    TRDOSWriter();
    ~TRDOSWriter();

    void addBasicFile(std::string name, const std::string& data, int startLine = -1);
    void addCodeFile(std::string name, const CodeEmitter::Byte* data, size_t size, size_t startAddress);

    void writeSclFile(const std::filesystem::path& path);
    void writeTrdFile(const std::filesystem::path& path, std::string volumeName);

private:
    class DiskFile;
    class BasicFile;
    class CodeFile;

    std::vector<std::unique_ptr<DiskFile>> mFiles;

    DISABLE_COPY(TRDOSWriter);
};

#endif
