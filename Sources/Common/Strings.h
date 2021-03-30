#ifndef COMMON_STRINGS_H
#define COMMON_STRINGS_H

#include "Common/Common.h"

std::string toLower(std::string str);

bool equalCaseInsensitive(const char* str1, const char* str2);

bool boolFromString(bool& out, const char* p, size_t size);
bool intFromString(int& out, const char* p, size_t size);

std::wstring wstringFromUtf8(const std::string& str);

bool startsWith(const std::string& str, const char* start);
bool startsWith(const std::string& str, const std::string& start);
bool endsWith(const std::string& str, const char* end);
bool endsWith(const std::string& str, const std::string& end);

#endif
