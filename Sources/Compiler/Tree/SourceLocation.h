#ifndef COMPILER_TREE_SOURCELOCATION_H
#define COMPILER_TREE_SOURCELOCATION_H

#include "Common/GC.h"

class SourceLocation : public GCObject
{
public:
    SourceLocation(const char* file, int line)
        : mFile(file)
        , mLine(line)
    {
    }

    const char* file() const { return mFile; }
    int line() const { return mLine; }

private:
    const char* mFile;
    int mLine;

    DISABLE_COPY(SourceLocation);
};

#endif
