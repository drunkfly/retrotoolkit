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

bool boolFromString(bool& out, const char* p, size_t size)
{
    switch (size) {
        case 1:
            if (*p == '0' || *p == 'n' || *p == 'N')
                return (out = false), true;
            if (*p == '1' || *p == 'y' || *p == 'Y')
                return (out = true), true;
            break;

        case 2:
            if (equalCaseInsensitive(p, "no"))
                return (out = false), true;
            if (equalCaseInsensitive(p, "on"))
                return (out = true), true;
            break;

        case 3:
            if (equalCaseInsensitive(p, "off"))
                return (out = false), true;
            if (equalCaseInsensitive(p, "yes"))
                return (out = true), true;
            break;

        case 4:
            if (equalCaseInsensitive(p, "true"))
                return (out = true), true;
            break;

        case 5:
            if (equalCaseInsensitive(p, "false"))
                return (out = false), true;
            break;
    }

    return false;
}

std::wstring wstringFromUtf8(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str.c_str());
}
