#ifndef COMPILER_SOURCEFILE_H
#define COMPILER_SOURCEFILE_H

#include "Compiler/Tree/SourceLocation.h"

enum class FileType
{
    Unknown,
    Asm,
    Java,
    Basic,
};

struct SourceFile
{
    FileType fileType;
    FileID* fileID;

    bool operator<(const SourceFile& other) const
    {
        if (fileType < other.fileType)
            return true;
        else if (fileType > other.fileType)
            return false;

        return (fileID->name() < other.fileID->name());
    }

    static FileType determineFileType(const std::filesystem::path& path);
};

#endif
