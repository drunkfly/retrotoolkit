#include "DataBlob.h"
#include "Compiler/Linker/CompiledOutput.h"

static const DataBlob dummy;

DataBlob::DataBlob()
{
}

DataBlob::DataBlob(const void* data, size_t length)
    : mData(reinterpret_cast<const char*>(data), length)
{
}

DataBlob::DataBlob(CompiledFile* file)
{
    if (file) {
        mData.resize(file->size());

        const CodeEmitter::Byte* p = file->data();
        const CodeEmitter::Byte* end = p + file->size();
        char* dst = &mData[0];
        while (p < end)
            *dst++ = (*p++).value;
    }
}

DataBlob::~DataBlob()
{
}

const DataBlob& DataBlob::fileData(const std::string& fileName) const
{
    auto it = mFileData.find(fileName);
    return (it != mFileData.end() ? *it->second : dummy);
}

void DataBlob::setFileData(const std::string& fileName, DataBlob blob)
{
    mFileData[fileName] = std::make_unique<DataBlob>(std::move(blob));
}

std::ostream& operator<<(std::ostream& stream, const DataBlob& str)
{
    static const char hex[] = "0123456789abcdef";

    stream << '[';
    for (char ch : str.data()) {
        auto v = unsigned(static_cast<unsigned char>(ch));
        stream << ' ' << hex[v >> 4] << hex[v & 15];
    }
    stream << " ]";

    return stream;
}
