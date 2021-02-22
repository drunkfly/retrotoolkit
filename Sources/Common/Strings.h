#ifndef COMMON_STRINGS_H
#define COMMON_STRINGS_H

#include <string>

std::string toLower(std::string str);

bool equalCaseInsensitive(const char* str1, const char* str2);

std::wstring wstringFromUtf8(const std::string& str);

#endif
