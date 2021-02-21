#ifndef COMMON_IO_H
#define COMMON_IO_H

#include <string>
#include <filesystem>

enum WriteFlag
{
    FailIfExists = 0x01,
    SkipIfExists = 0x02,
    SkipIfSameContent = 0x04,
};

std::string loadFile(const std::filesystem::path& fileName);

void writeFile(const std::filesystem::path& fileName, const char* str, int flags = 0);
void writeFile(const std::filesystem::path& fileName, const std::string& str, int flags = 0);
void writeFile(const std::filesystem::path& fileName, const void* data, size_t size, int flags = 0);

#endif
