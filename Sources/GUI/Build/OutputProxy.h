#ifndef GUI_BUILD_OUTPUTPROXY_H
#define GUI_BUILD_OUTPUTPROXY_H

#include "GUI/Common.h"
#include "Compiler/Output/IOutputWriterProxy.h"

class Emulator;

class OutputProxy : public IOutputWriterProxy
{
public:
    explicit OutputProxy(std::shared_ptr<Emulator> emulator);
    ~OutputProxy();

    void setOutput(Project::Output::Type type, IOutputWriter* writer) override;

    void addBasicFile(SourceLocation* location, std::string name, const std::string& data, int startLine) override;
    void addCodeFile(SourceLocation* location, std::string name,
        const std::string& originalName, const CodeEmitter::Byte* data, size_t size, size_t startAddress) override;

    void writeOutput() override;

private:
    std::shared_ptr<Emulator> mEmulator;
    IOutputWriter* mWriter;

    DISABLE_COPY(OutputProxy);
};

#endif
