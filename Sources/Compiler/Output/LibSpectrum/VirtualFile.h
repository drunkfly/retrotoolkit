#ifndef COMPILER_OUTPUT_LIBSPECTRUM_VIRTUALFILE_H
#define COMPILER_OUTPUT_LIBSPECTRUM_VIRTUALFILE_H

#include "Common/Common.h"

#ifdef _WIN32
#define ssize_t ptrdiff_t
#endif

#include <File.h> // libaudiofile
#include <af_vfs.h>

class VirtualFile
{
public:
    VirtualFile();
    ~VirtualFile();

    const void* data() const { return mBuffer.get(); }
    size_t length() const { return size_t(mSize); }

    operator AFvirtualfile*() const { return mHandle; }

private:
    AFvirtualfile* mHandle;
    std::unique_ptr<uint8_t[]> mBuffer;
    AFfileoffset mOffset;
    AFfileoffset mSize;
    size_t mBufferSize;
    int mDestroyed;

    static void destroy(AFvirtualfile* vf);
    static AFfileoffset tell(AFvirtualfile* vf);
    static AFfileoffset seek(AFvirtualfile* vf, AFfileoffset offset, int whence);
    static ssize_t write(AFvirtualfile* vf, const void* data, size_t nbytes);

    DISABLE_COPY(VirtualFile);
};

#endif
