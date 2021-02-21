#include "Conversion.h"
#include <locale>
#include <codecvt>

std::wstring fromUtf8(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str.c_str());
}
