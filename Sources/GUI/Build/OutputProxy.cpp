#include "OutputProxy.h"

OutputProxy::OutputProxy(std::shared_ptr<Emulator> emulator)
    : mEmulator(std::move(emulator))
    , mWriter(nullptr)
{
    // FIXME
    //mMemory.reset(new uint8_t[Z80Memory::BankCount * Z80Memory::BankSize]);
    //mState.reset(new SnapshotState);
}

OutputProxy::~OutputProxy()
{
}

void OutputProxy::setOutput(Project::Output::Type type, IOutputWriter* writer)
{
    mWriter = writer;
}

void OutputProxy::addBasicFile(SourceLocation* location, std::string name, const std::string& data, int startLine)
{
    mWriter->addBasicFile(location, std::move(name), data, startLine);
}

void OutputProxy::addCodeFile(SourceLocation* location, std::string name,
    const std::string& originalName, const CodeEmitter::Byte* data, size_t size, size_t startAddress)
{
    mWriter->addCodeFile(location, std::move(name), originalName, data, size, startAddress);
}

void OutputProxy::writeOutput()
{
    mWriter->writeOutput();
}
