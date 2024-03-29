#ifndef COMPILER_OUTPUT_IOUTPUTWRITER_H
#define COMPILER_OUTPUT_IOUTPUTWRITER_H

#include "Compiler/Linker/CodeEmitter.h"

class IOutputWriter
{
public:
    virtual ~IOutputWriter() = default;

    virtual void addBasicFile(SourceLocation* location, std::string name,
        const std::string& data, int startLine = -1) = 0;

    virtual void addCodeFile(SourceLocation* location, std::string name,
        const std::string& originalName, const CodeEmitter::Byte* data, size_t size, size_t startAddress) = 0;

    virtual void writeOutput() = 0;
};

#endif
