#ifndef COMMON_IO_H
#define COMMON_IO_H

#include <string>

enum WriteFlag
{
    FailIfExists = 0x01,
    SkipIfExists = 0x02,
    SkipIfSameContent = 0x04,
};

std::string loadFile(const std::string& fileName);

void writeFile(const std::string& fileName, const char* str, int flags = 0);
void writeFile(const std::string& fileName, const std::string& str, int flags = 0);
void writeFile(const std::string& fileName, const void* data, size_t size, int flags = 0);

#endif
