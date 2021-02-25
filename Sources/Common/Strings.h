#ifndef COMMON_STRINGS_H
#define COMMON_STRINGS_H

#include <string>

std::string toLower(std::string str);

bool equalCaseInsensitive(const char* str1, const char* str2);

bool boolFromString(bool& out, const char* p, size_t size);

std::wstring wstringFromUtf8(const std::string& str);

bool endsWith(const std::string& str, const std::string& end);

#endif
