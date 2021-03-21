#include "VirtualFile.h"
#include <assert.h>
#include <string.h>

VirtualFile::VirtualFile()
    : mOffset(0)
    , mSize(0)
    , mBufferSize(0)
    , mDestroyed(0)
{
    mHandle = af_virtual_file_new();
    if (!mHandle)
        return;

    mHandle->closure = this;
    mHandle->destroy = destroy;
    mHandle->tell = tell;
    mHandle->length = tell;
    mHandle->seek = seek;
    mHandle->write = write;
}

VirtualFile::~VirtualFile()
{
    if (mHandle) {
        assert(mDestroyed == 0 || mDestroyed == 1);
        if (!mDestroyed) {
            af_virtual_file_destroy(mHandle);
            assert(mDestroyed == 1);
        }
    }
}

void VirtualFile::destroy(AFvirtualfile* vf)
{
    auto self = reinterpret_cast<VirtualFile*>(vf->closure);
    assert(self->mDestroyed == 0);
    self->mDestroyed++;
}

AFfileoffset VirtualFile::tell(AFvirtualfile* vf)
{
    auto self = reinterpret_cast<VirtualFile*>(vf->closure);
    assert(self->mDestroyed == 0);
    return self->mOffset;
}

AFfileoffset VirtualFile::seek(AFvirtualfile* vf, AFfileoffset offset, int whence)
{
    auto self = reinterpret_cast<VirtualFile*>(vf->closure);
    assert(self->mDestroyed == 0);

    AFfileoffset newOffset;
    switch (whence) {
        case File::SeekFromBeginning: newOffset = offset; break;
        case File::SeekFromCurrent: newOffset = self->mOffset + offset; break;
        case File::SeekFromEnd: newOffset = self->mSize + offset; break;
        default: assert(false); return -1;
    }

    self->mOffset = newOffset;
    return newOffset;
}

ssize_t VirtualFile::write(AFvirtualfile* vf, const void* data, size_t nbytes)
{
    auto self = reinterpret_cast<VirtualFile*>(vf->closure);
    assert(self->mDestroyed == 0);

    AFfileoffset curOffset = self->mOffset;
    AFfileoffset newOffset = curOffset + nbytes;
    if (newOffset >= self->mSize) {
        size_t newLength = self->mBufferSize;
        if (newLength == 0)
            newLength = 16384;
        while (newLength < newOffset)
            newLength *= 2;

        std::unique_ptr<uint8_t[]> newBuffer{new uint8_t[newLength]};
        if (self->mBufferSize != 0)
            memcpy(newBuffer.get(), self->mBuffer.get(), self->mBufferSize);

        self->mBuffer = std::move(newBuffer);
        self->mBufferSize = newLength;
        self->mSize = newOffset;
    }

    memcpy(self->mBuffer.get() + curOffset, data, nbytes);
    self->mOffset = newOffset;

    return ssize_t(nbytes);
}
