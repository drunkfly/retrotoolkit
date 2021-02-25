#include "CompiledOutput.h"

CompiledFile::CompiledFile(std::string name)
    : mName(std::move(name))
    , mLoadAddress(0)
    , mUsedByBasic(false)
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

CompiledFile* CompiledOutput::getFile(const std::string& name)
{
    auto it = mFiles.find(name);
    return (it != mFiles.end() ? it->second : nullptr);
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
