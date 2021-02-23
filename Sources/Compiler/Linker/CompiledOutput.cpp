#include "CompiledOutput.h"

CompiledFile::CompiledFile(std::string name)
    : mName(std::move(name))
{
    registerFinalizer();
}

CompiledFile::~CompiledFile()
{
}

void CompiledFile::emitByte(SourceLocation* location, uint8_t byte)
{
    mBytes.emplace_back(Byte{ location, byte });
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CompiledOutput::CompiledOutput()
{
    registerFinalizer();
}

CompiledOutput::~CompiledOutput()
{
}

CompiledFile* CompiledOutput::getOrAddFile(const std::string& name)
{
    auto it = mFiles.find(name);
    if (it != mFiles.end())
        return it->second;

    CompiledFile* file = new (heap()) CompiledFile(name);
    mFiles[name] = file;

    return file;
}
