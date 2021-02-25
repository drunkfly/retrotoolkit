#ifndef COMPILER_SOURCEFILE_H
#define COMPILER_SOURCEFILE_H

#include "Compiler/Tree/SourceLocation.h"

enum class FileType
{
    Asm,
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
};

#endif
