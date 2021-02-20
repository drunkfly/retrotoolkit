#include "IO.h"
#include "Common/Common.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <stdexcept>
#include <locale>
#include <codecvt>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
 #include <direct.h>
 using StringType = std::wstring;
#else
 #include <unistd.h>
 using StringType = std::string;
#endif

namespace
{
    class FileHandle
    {
    public:
        FileHandle(FILE* f, const StringType& fileName) noexcept
            : mHandle(f)
            , mFileName(fileName)
            , mDeleteOnClose(false)
        {
        }

        ~FileHandle() noexcept
        {
            fclose(mHandle);
            if (mDeleteOnClose) {
              #ifdef _WIN32
                _wremove(mFileName.c_str());
              #else
                remove(mFileName.c_str());
              #endif
            }
        }

        void setDeleteOnClose(bool flag) { mDeleteOnClose = flag; }

        operator FILE*() const noexcept { return mHandle; }
        bool operator!() const noexcept { return !mHandle; }

    private:
        FILE* mHandle;
        const StringType& mFileName;
        bool mDeleteOnClose;

        DISABLE_COPY(FileHandle);
    };
}

static void error(const char* message, const std::string& fileName)
{
    const char* error = strerror(errno);
    std::stringstream ss;

    for (const char* p = message; *p; ) {
        if (*p != '%') {
            ss << *p++;
            continue;
        }

        ++p;
        switch (*p++) {
            case 'f':
                ss << '"' << fileName << '"';
                break;
            case 'e':
                ss << error;
                break;
        }
    }

    throw std::runtime_error(ss.str());
}

std::string loadFile(const std::string& fileName)
{
  #ifdef _WIN32
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring fileNameW = converter.from_bytes(fileName.c_str());
    FileHandle f{_wfopen(fileNameW.c_str(), L"rb"), fileNameW};
  #else
    FileHandle f{fopen(fileName.c_str(), "rb"), fileName};
  #endif
    if (!f)
        error("Unable to open file %f: %e", fileName);

    if (fseek(f, 0, SEEK_END) != 0)
        error("Seek failed in file %f: %e", fileName);

    long size = ftell(f);
    if (size < 0)
        error("Unable to determine size of file %f: %e", fileName);

    if (fseek(f, 0, SEEK_SET) != 0)
        error("Seek failed in file %f: %e", fileName);

    std::string buffer(size_t(size), 0);
    size_t bytesRead = fread(&buffer[0], 1, size_t(size), f);
    if (ferror(f))
        error("Unable to read file %f: %e", fileName);
    if (bytesRead != size)
        error("Unexpected end of file %f.", fileName);

    return buffer;
}

void writeFile(const std::string& fileName, const char* str, int flags)
{
    return writeFile(fileName, str, strlen(str), flags);
}

void writeFile(const std::string& fileName, const std::string& str, int flags)
{
    return writeFile(fileName, str.data(), str.length(), flags);
}

void writeFile(const std::string& fileName, const void* data, size_t size, int flags)
{
  #ifdef _WIN32
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring fileNameW = converter.from_bytes(fileName.c_str());

    const wchar_t* p = fileNameW.c_str();
    for (;;) {
        const wchar_t* end = wcschr(p, L'/');
        const wchar_t* end2 = wcschr(p, L'\\');
        if (end2 && (!end || end2 < end))
            end = end2;
        if (!end)
            break;
        _wmkdir(std::wstring(p, end - p).c_str());
        p = end + 1;
    }

    struct _stat st;
    int r = _wstat(fileNameW.c_str(), &st);
    bool fileExists = (r == 0);
  #else
    const char* p = fileName.c_str();
    for (;;) {
        const char* end = strchr(p, '/');
        if (!end)
            break;
        mkdir(std::string(p, end - p).c_str(), 0777);
        p = end + 1;
    }

    struct stat st;
    int r = stat(fileName.c_str(), &st);
    bool fileExists = (r == 0);
  #endif

    bool skipIfExists = (flags & SkipIfExists) != 0;
    if ((skipIfExists || (flags & FailIfExists) != 0) && fileExists) {
        if (skipIfExists)
            return;
        else
            error("File %f already exists.", fileName);
    }

    if (fileExists && (flags & SkipIfSameContent) != 0) {
        try {
            std::string oldContent = loadFile(fileName);
            if (oldContent.size() == size && memcmp(oldContent.data(), data, size) == 0)
                return;
        } catch (...) {
        }
    }

  #ifdef _WIN32
    FileHandle f{_wfopen(fileNameW.c_str(), L"wb"), fileNameW};
  #else
    FileHandle f{fopen(fileName.c_str(), "wb"), fileName};
  #endif
    if (!f)
        error("Unable to open file %f for writing: %e", fileName);

    f.setDeleteOnClose(true);

    size_t bytesWritten = fwrite(data, 1, size, f);
    if (ferror(f))
        error("Unable to write file %f: %e", fileName);
    if (bytesWritten != size)
        error("Incomplete write in file %f.", fileName);

    f.setDeleteOnClose(false);
}
