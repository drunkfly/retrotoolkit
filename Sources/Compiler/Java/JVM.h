#ifndef JVM_H
#define JVM_H

#include "Common/Common.h"
#include "Compiler/Java/JNI.h"
#include <filesystem>

class JStringList;

class JVM
{
public:
    static bool isLoaded();
    static const std::filesystem::path& loadedDllPath();

    static JNIEnv* jniEnv();

    static std::filesystem::path findJvmDll(const std::filesystem::path& jdkPath);
    static std::filesystem::path findJavaC(const std::filesystem::path& jdkPath);

    static void load(std::filesystem::path dllPath);
    static void destroy();

    static bool isAttached();
    static void attachCurrentThread();
    static void detachCurrentThread();

    static void throwIfException();

    static jstring toJString(const std::string& utf8);
    static jstring toJString(const std::wstring& str);
    static jstring toJString(const std::filesystem::path& str);

    static std::string toUtf8(jstring str);
    static std::wstring toWString(jstring str);
    static std::filesystem::path toPath(jstring str);

    static jstring className(jobject obj);

    static jclass stringClass();

    static bool compile(const JStringList& args);

private:
    DISABLE_COPY(JVM);
};

#endif
