#include "IO.h"
#include "Common/Common.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

namespace
{
    class FileHandle
    {
    public:
        FileHandle(FILE* f, const std::filesystem::path& fileName) noexcept
            : mHandle(f)
            , mFileName(fileName)
            , mDeleteOnClose(false)
        {
        }

        ~FileHandle() noexcept
        {
            fclose(mHandle);
            if (mDeleteOnClose) {
                std::error_code error;
                std::filesystem::remove(mFileName, error);
            }
        }

        void setDeleteOnClose(bool flag) { mDeleteOnClose = flag; }

        operator FILE*() const noexcept { return mHandle; }
        bool operator!() const noexcept { return !mHandle; }

    private:
        FILE* mHandle;
        const std::filesystem::path& mFileName;
        bool mDeleteOnClose;

        DISABLE_COPY(FileHandle);
    };
}

static void error(const char* message, const std::filesystem::path& fileName)
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

std::string loadFile(const std::filesystem::path& fileName)
{
  #ifdef _WIN32
    FileHandle f{_wfopen(fileName.c_str(), L"rb"), fileName};
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

void writeFile(const std::filesystem::path& fileName, const char* str, int flags)
{
    return writeFile(fileName, str, strlen(str), flags);
}

void writeFile(const std::filesystem::path& fileName, const std::string& str, int flags)
{
    return writeFile(fileName, str.data(), str.length(), flags);
}

void writeFile(const std::filesystem::path& fileName, const void* data, size_t size, int flags)
{
    std::filesystem::path dir = fileName;
    dir.remove_filename();
    std::filesystem::create_directories(dir);

    auto status = std::filesystem::status(fileName);
    bool fileExists = std::filesystem::exists(status) && !std::filesystem::is_directory(status);

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
    FileHandle f{_wfopen(fileName.c_str(), L"wb"), fileName};
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
