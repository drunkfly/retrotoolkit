#ifndef COMPILER_JAVA_JNISTRINGREF_H
#define COMPILER_JAVA_JNISTRINGREF_H

#include "Compiler/Java/JNIRef.h"

class JNIStringRef final : public JNIRef
{
    JNIREF(JNIStringRef, jstring)

public:
    static JNIStringRef from(const char* utf8);
    static JNIStringRef from(const std::string& utf8);
    static JNIStringRef from(const std::wstring& str);
    static JNIStringRef from(const std::filesystem::path& str);

    std::string toUtf8() const;
    std::wstring toWString() const;
    std::filesystem::path toPath() const;

    DISABLE_COPY(JNIStringRef);
};

#endif
