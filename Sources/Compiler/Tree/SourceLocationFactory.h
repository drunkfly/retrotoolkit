#ifndef COMPILER_TREE_SOURCELOCATIONFACTORY_H
#define COMPILER_TREE_SOURCELOCATIONFACTORY_H

#include "Compiler/Tree/SourceLocation.h"

class SourceLocationFactory
{
public:
    explicit SourceLocationFactory(GCHeap* heap);
    ~SourceLocationFactory();

    void setFileName(std::filesystem::path name, std::filesystem::path path);
    SourceLocation* createLocation(int line) const;

private:
    GCHeap* mHeap;
    FileID* mFileID;

    DISABLE_COPY(SourceLocationFactory);
};

#endif
