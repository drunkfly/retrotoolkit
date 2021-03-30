#ifndef COMPILER_JAVA_JSTRINGLIST_H
#define COMPILER_JAVA_JSTRINGLIST_H

#include "Compiler/Java/JNI.h"

class JStringList
{
public:
    struct Argument
    {
        std::optional<std::string> string;
        std::optional<std::wstring> wstring;
        std::optional<std::filesystem::path> path;
    };

    JStringList();
    ~JStringList();

    void clear();

    void reserve(size_t size);

    void add(const char* str);
    void add(const wchar_t* str);
    void add(std::string str);
    void add(std::wstring wstr);
    void add(std::filesystem::path path);

    jobjectArray toJavaArray() const;

private:
    std::vector<Argument> mArguments;
};

#endif
