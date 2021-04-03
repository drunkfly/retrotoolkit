#ifndef COMPILER_JAVA_JVM_H
#define COMPILER_JAVA_JVM_H

#include "Compiler/Java/JNI.h"

class JStringList;
class ICompilerListener;

class JVM
{
public:
    static bool isLoaded();
    static const std::filesystem::path& loadedDllPath();
    static bool loadedVerboseGC();
    static bool loadedVerboseClass();
    static bool loadedVerboseJNI();

    static JNIEnv* jniEnv();

    static std::filesystem::path findJvmDll(const std::filesystem::path& jdkPath);
    static std::filesystem::path findJavaC(const std::filesystem::path& jdkPath);
    static std::filesystem::path findToolsJar(const std::filesystem::path& jdkPath);

    static void setListener(ICompilerListener* listener);
    static void setVerboseGC(bool flag);
    static void setVerboseClass(bool flag);
    static void setVerboseJNI(bool flag);

    static void load(const std::filesystem::path& jdkPath);
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

    static jobject makeGlobalRef(jobject local);

    static jstring className(jobject obj);

    static jclass stringClass();

    static bool compile(const JStringList& args);
    static bool runClass(const char* className, const JStringList& args,
        bool useClassLoader = false, const JStringList* classPath = nullptr);

private:
    static void JNICALL drunkfly_Messages_print(JNIEnv* env, jclass, jstring message);
    static jobject JNICALL drunkfly_Messages_getInstance(JNIEnv* env, jclass);
    static jobject JNICALL drunkfly_Messages_getPrintWriter(JNIEnv* env, jclass);
    static jobject JNICALL drunkfly_BuilderClassLoader_getInstance(JNIEnv* env, jclass);

    static void ensureNecessaryClassesLoaded();

    static int vfprintfHook(FILE* fp, const char* format, va_list args);
    static void exitHook(int code);
    static void abortHook();

    DISABLE_COPY(JVM);
};

#endif
