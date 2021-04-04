#ifndef COMPILER_JAVA_JVM_H
#define COMPILER_JAVA_JVM_H

#include "Compiler/Java/JNI.h"

class JStringList;
class JNIClassRef;

class JVM
{
public:
    class AttachCurrentThread
    {
    public:
        AttachCurrentThread() { JVM::attachCurrentThread(); }
        ~AttachCurrentThread() { JVM::detachCurrentThread(); }
        DISABLE_COPY(AttachCurrentThread);
    };

    static JNIEnv* jniEnv();

    static std::filesystem::path findJvmDll(const std::filesystem::path& jdkPath);
    static std::filesystem::path findJavaC(const std::filesystem::path& jdkPath);
    static std::filesystem::path findToolsJar(const std::filesystem::path& jdkPath);

    static void setVerboseGC(bool flag);
    static void setVerboseClass(bool flag);
    static void setVerboseJNI(bool flag);
    static bool loadedVerboseGC();
    static bool loadedVerboseClass();
    static bool loadedVerboseJNI();

    static bool isLoaded();
    static const std::filesystem::path& loadedDllPath();
    static void load(const std::filesystem::path& jdkPath, const std::filesystem::path& classPath);
    static void unloadPermanently();

    static bool isAttached();
    static void attachCurrentThread();
    static void detachCurrentThread();

    static void printJniError(std::stringstream& ss, int r);

    static int majorVersion();

    static bool compile(const JStringList& args);
    static bool runClass(const char* className, const JStringList& args,
        bool useClassLoader = false, const JStringList* classPath = nullptr);

private:
    static int vfprintfHook(FILE* fp, const char* format, va_list args);
    static void exitHook(int code);
    static void abortHook();

    DISABLE_COPY(JVM);
};

#endif
