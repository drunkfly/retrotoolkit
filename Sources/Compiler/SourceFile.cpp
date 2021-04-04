#include "SourceFile.h"

FileType SourceFile::determineFileType(const std::filesystem::path& path)
{
    auto ext = path.extension();
    if (ext == ".asm")
        return FileType::Asm;
    else if (ext == ".java")
        return FileType::Java;
    else if (ext == ".bas")
        return FileType::Basic;
    else
        return FileType::Unknown;
}
