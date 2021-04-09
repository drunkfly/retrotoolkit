#include "CompiledOutput.h"
#include "Compiler/Linker/DebugInformation.h"
#include "Compiler/CompilerError.h"

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

CompiledFile* CompiledOutput::addFile(SourceLocation* location, SourceLocation* nameLocation,
    const std::string& name, std::unique_ptr<DebugInformation> debugInfo)
{
    auto it = mFiles.find(name);
    if (it != mFiles.end()) {
        std::stringstream ss;
        ss << "File name is not unique: \"" << name << "\".";
        throw CompilerError(nameLocation, ss.str());
    }

    CompiledFile* file = new (heap()) CompiledFile(location, name, std::move(debugInfo));
    mFiles[name] = file;
    mFileList.emplace_back(file);

    return file;
}
