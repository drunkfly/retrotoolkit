#include "SourceLocationFactory.h"

SourceLocationFactory::SourceLocationFactory(GCHeap* heap)
    : mHeap(heap)
{
}

SourceLocationFactory::~SourceLocationFactory()
{
}

void SourceLocationFactory::setFileName(std::filesystem::path name, std::filesystem::path path)
{
    mFileID = new (mHeap) FileID(std::move(name), std::move(path));
}

SourceLocation* SourceLocationFactory::createLocation(int line) const
{
    assert(mFileID != nullptr);
    if (!mFileID)
        return nullptr;
    return new (mHeap) SourceLocation(mFileID, line);
}
