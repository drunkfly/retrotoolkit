#include "Strings.h"
#include <locale>
#include <codecvt>
#include <ctype.h>

std::string toLower(std::string str)
{
    for (auto& ch : str)
        ch = tolower(ch);
    return str;
}

bool equalCaseInsensitive(const char* str1, const char* str2)
{
    for (;;) {
        char c1 = *str1++;
        char c2 = *str2++;
        if (c1 == 0 || c2 == 0)
            return c1 == c2;
        if (c1 != c2) {
            c1 = tolower((unsigned char)c1);
            c2 = tolower((unsigned char)c2);
            if (c1 != c2)
                return false;
        }
    }
}

std::wstring wstringFromUtf8(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str.c_str());
}
