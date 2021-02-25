#include "CompiledOutput.h"

CompiledFile::CompiledFile(std::string name)
    : mName(std::move(name))
{
    registerFinalizer();
}

CompiledFile::~CompiledFile()
{
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
    mFileList.emplace_back(file);

    return file;
}
