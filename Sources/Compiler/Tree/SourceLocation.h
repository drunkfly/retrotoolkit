#ifndef COMPILER_TREE_SOURCELOCATION_H
#define COMPILER_TREE_SOURCELOCATION_H

#include "Common/GC.h"
#include <filesystem>

class FileID : public GCObject
{
public:
    explicit FileID(std::filesystem::path path)
        : mPath(std::move(path))
    {
        registerFinalizer();
    }

    const std::filesystem::path& path() const { return mPath; }

private:
    std::filesystem::path mPath;

    DISABLE_COPY(FileID);
};

class SourceLocation : public GCObject
{
public:
    SourceLocation(const FileID* file, int line)
        : mFile(file)
        , mLine(line)
    {
    }

    const FileID* file() const { return mFile; }
    int line() const { return mLine; }

private:
    const FileID* mFile;
    int mLine;

    DISABLE_COPY(SourceLocation);
};

#endif
