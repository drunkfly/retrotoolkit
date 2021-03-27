#include "JVM.h"
#include "Compiler/Java/JStringList.h"
#include "Compiler/CompilerError.h"
#include <vector>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
 #define WIN32_LEAN_AND_MEAN
 #define NOMINMAX
 #include <windows.h>
 static HMODULE jvmDll;
#else
 #include <dlfcn.h>
 static void* jvmDll;
#endif

static JavaVM* jvm;
static JNIEnv* env;
static std::filesystem::path loadedDllPath;
static jclass stringClassRef;

bool JVM::isLoaded()
{
    return jvmDll || jvm || env;
}

const std::filesystem::path& JVM::loadedDllPath()
{
    return ::loadedDllPath;
}

JNIEnv* JVM::jniEnv()
{
    return env;
}

std::filesystem::path JVM::findJvmDll(const std::filesystem::path& jdkPath)
{
    std::filesystem::path jvmDllPath;

  #ifdef _WIN32
    jvmDllPath = jdkPath / "jre/bin/client/jvm.dll";
    if (std::filesystem::exists(jvmDllPath) && std::filesystem::is_regular_file(jvmDllPath))
        return jvmDllPath;

    jvmDllPath = jdkPath / "jre/bin/server/jvm.dll";
    if (std::filesystem::exists(jvmDllPath) && std::filesystem::is_regular_file(jvmDllPath))
        return jvmDllPath;

    jvmDllPath = jdkPath / "bin/client/jvm.dll";
    if (std::filesystem::exists(jvmDllPath) && std::filesystem::is_regular_file(jvmDllPath))
        return jvmDllPath;

    jvmDllPath = jdkPath / "bin/server/jvm.dll";
    if (std::filesystem::exists(jvmDllPath) && std::filesystem::is_regular_file(jvmDllPath))
        return jvmDllPath;
  #elif defined(__linux__)
    if (sizeof(void*) == 4) {
        jvmDllPath = jdkPath / "jre/lib/i386/client/libjvm.so";
        if (std::filesystem::exists(jvmDllPath) && std::filesystem::is_regular_file(jvmDllPath))
            return jvmDllPath;

        jvmDllPath = jdkPath / "jre/lib/i386/server/libjvm.so";
        if (std::filesystem::exists(jvmDllPath) && std::filesystem::is_regular_file(jvmDllPath))
            return jvmDllPath;
    } else if (sizeof(void*) == 8) {
        jvmDllPath = jdkPath / "jre/lib/amd64/client/libjvm.so";
        if (std::filesystem::exists(jvmDllPath) && std::filesystem::is_regular_file(jvmDllPath))
            return jvmDllPath;

        jvmDllPath = jdkPath / "jre/lib/amd64/server/libjvm.so";
        if (std::filesystem::exists(jvmDllPath) && std::filesystem::is_regular_file(jvmDllPath))
            return jvmDllPath;
    }

    jvmDllPath = jdkPath / "lib/client/libjvm.so";
    if (std::filesystem::exists(jvmDllPath) && std::filesystem::is_regular_file(jvmDllPath))
        return jvmDllPath;

    jvmDllPath = jdkPath / "lib/server/libjvm.so";
    if (std::filesystem::exists(jvmDllPath) && std::filesystem::is_regular_file(jvmDllPath))
        return jvmDllPath;
  #endif

    // FIXME: Linux, MacOS

    std::stringstream ss;
    ss << "Unable to find JVM shared library in \"" << jdkPath.string() << "\".";
    throw CompilerError(nullptr, ss.str());
}

std::filesystem::path JVM::findJavaC(const std::filesystem::path& jdkPath)
{
    std::filesystem::path javacPath;

  #ifdef _WIN32
    javacPath = jdkPath / "bin/javac.exe";
    if (std::filesystem::exists(javacPath) && std::filesystem::is_regular_file(javacPath))
        return javacPath;
  #elif defined(__linux__)
    javacPath = jdkPath / "bin/java";
    if (std::filesystem::exists(javacPath) && std::filesystem::is_regular_file(javacPath))
        return javacPath;
  #endif

    // FIXME: Linux, MacOS

    std::stringstream ss;
    ss << "Unable to find Java compiler executable in \"" << jdkPath.string() << "\".";
    throw CompilerError(nullptr, ss.str());
}

void JVM::load(std::filesystem::path dllPath)
{
    if (!jvmDll) {
      #ifdef _WIN32
        jvmDll = LoadLibraryW(dllPath.c_str());
        if (!jvmDll) {
            DWORD dwLastError = GetLastError();
            std::stringstream ss;
            ss << "Unable to load library \"" << dllPath.string()
                << "\" (code 0x" << std::hex << std::setw(8) << std::setfill('0') << dwLastError << ").";
            throw CompilerError(nullptr, ss.str());
        }
      #else
        jvmDll = dlopen(dllPath.c_str(), RTLD_NOW | RTLD_GLOBAL);
        if (!jvmDll) {
            std::stringstream ss;
            ss << "Unable to load library \"" << dllPath.string() << "\": " << dlerror();
            throw CompilerError(nullptr, ss.str());
        }
      #endif
        ::loadedDllPath = std::move(dllPath);
    }

    if (!jvm) {
      #ifdef _WIN32
        auto JNI_CreateJavaVM = (PFNJNICREATEJAVAVM)GetProcAddress(jvmDll, "JNI_CreateJavaVM");
        if (!JNI_CreateJavaVM) {
            DWORD dwLastError = GetLastError();
            destroy();
            std::stringstream ss;
            ss << "Unable to resolve symbol \"JNI_CreateJavaVM\" in library \"" << ::loadedDllPath.string()
                << "\" (code 0x" << std::hex << std::setw(8) << std::setfill('0') << dwLastError << ").";
            throw CompilerError(nullptr, ss.str());
        }
      #else
        dlerror();
        auto JNI_CreateJavaVM = (PFNJNICREATEJAVAVM)dlsym(jvmDll, "JNI_CreateJavaVM");
        if (!JNI_CreateJavaVM) {
            std::string error = dlerror();
            destroy();
            std::stringstream ss;
            ss << "Unable to resolve symbol \"JNI_CreateJavaVM\" in library \""
                << ::loadedDllPath.string() << "\": " << error;
            throw CompilerError(nullptr, ss.str());
        }
      #endif

        std::vector<JavaVMOption> opts;
      #ifndef NDEBUG
        opts.emplace_back(JavaVMOption{ "-verbose:jni" });
      #endif

        JavaVMInitArgs args;
        args.version = JNI_VERSION_1_4;
        args.options = opts.data();
        args.nOptions = int(opts.size());
        args.ignoreUnrecognized = false;

        int r = JNI_CreateJavaVM(&jvm, &env, &args);
        if (r != JNI_OK) {
            std::stringstream ss;
            ss << "Unable to initialize Java Virtual Machine: ";
            switch (r) {
                case JNI_ERR: ss << "JNI_ERR"; break;
                case JNI_EDETACHED: ss << "JNI_EDETACHED"; break;
                case JNI_EVERSION: ss << "JNI_EVERSION"; break;
                case JNI_ENOMEM: ss << "JNI_ENOMEM"; break;
                case JNI_EEXIST: ss << "JNI_EEXIST"; break;
                case JNI_EINVAL: ss << "JNI_EINVAL"; break;
                default: ss << "code " << r;
            }
            throw CompilerError(nullptr, ss.str());
        }
    }
}

void JVM::destroy()
{
    if (jvm) {
        jvm->vtbl->DestroyJavaVM(jvm);
        jvm = nullptr;
        env = nullptr;
    }

    if (jvmDll) {
      #ifdef _WIN32
        FreeLibrary(jvmDll);
      #else
        dlclose(jvmDll);
      #endif
        jvmDll = nullptr;
    }

    stringClassRef = nullptr;
}

void JVM::throwIfException()
{
    if (!env->vtbl->ExceptionCheck(env))
        return;

    jthrowable throwable = env->vtbl->ExceptionOccurred(env);
    env->vtbl->ExceptionClear(env);
    if (!throwable)
        throw CompilerError(nullptr, "Unknown Java exception.");

    jstring throwableClassName = className(throwable);
    if (!throwableClassName) {
        env->vtbl->DeleteLocalRef(env, throwable);
        throw CompilerError(nullptr, "Unknown Java exception.");
    }

    std::string classNameString = toUtf8(throwableClassName);
    env->vtbl->DeleteLocalRef(env, throwableClassName);

    size_t index = classNameString.rfind('.');
    if (index != std::string::npos)
        classNameString = classNameString.substr(index + 1);

    jclass throwableClass = env->vtbl->GetObjectClass(env, throwable);
    if (!throwableClass) {
        env->vtbl->DeleteLocalRef(env, throwable);
        throw CompilerError(nullptr, "Unknown Java exception.");
    }

    jmethodID throwableGetMessageMethod =
        env->vtbl->GetMethodID(env, throwableClass, "getMessage", "()Ljava/lang/String;");
    env->vtbl->DeleteLocalRef(env, throwableClass);
    if (!throwableGetMessageMethod) {
        env->vtbl->DeleteLocalRef(env, throwable);
        std::stringstream ss;
        ss << "JVM: " << classNameString;
        throw CompilerError(nullptr, ss.str());
    }

    jstring message = (jstring)env->vtbl->CallObjectMethod(env, throwable, throwableGetMessageMethod);
    if (!message) {
        env->vtbl->DeleteLocalRef(env, throwable);
        std::stringstream ss;
        ss << "JVM: " << classNameString;
        throw CompilerError(nullptr, ss.str());
    }

    std::string messageString = toUtf8(message);
    env->vtbl->DeleteLocalRef(env, message);
    env->vtbl->DeleteLocalRef(env, throwable);

    std::stringstream ss;
    ss << "JVM: " << classNameString << ": " << messageString;
    throw CompilerError(nullptr, ss.str());
}

jstring JVM::toJString(const std::string& utf8)
{
    return env->vtbl->NewStringUTF(env, utf8.c_str());
}

jstring JVM::toJString(const std::wstring& str)
{
    size_t n = str.length();
    if constexpr (sizeof(jchar) == sizeof(wchar_t))
        return env->vtbl->NewString(env, reinterpret_cast<const jchar*>(str.c_str()), jint(n));
    else {
        std::unique_ptr<jchar[]> buf{new (std::nothrow) jchar[n]};
        if (!buf.get())
            return nullptr;
        for (size_t i = 0; i < n; i++)
            buf[i] = str[i];
        return env->vtbl->NewString(env, buf.get(), n);
    }
}

jstring JVM::toJString(const std::filesystem::path& str)
{
    return toJString(str.native());
}

std::string JVM::toUtf8(jstring str)
{
    if (!str)
        return std::string();

    jint length = env->vtbl->GetStringUTFLength(env, str);
    if (length <= 0)
        return std::string();

    const char* utf = env->vtbl->GetStringUTFChars(env, str, nullptr);
    if (!utf)
        return std::string();

    std::string result(utf, size_t(length));
    env->vtbl->ReleaseStringUTFChars(env, str, utf);

    return result;
}

std::wstring JVM::toWString(jstring str)
{
    if (!str)
        return std::wstring();

    jint length = env->vtbl->GetStringLength(env, str);
    if (length <= 0)
        return std::wstring();

    const jchar* chars = env->vtbl->GetStringChars(env, str, nullptr);
    if (!chars)
        return std::wstring();

    if constexpr (sizeof(jchar) == sizeof(wchar_t)) {
        std::wstring result(reinterpret_cast<const wchar_t*>(chars), size_t(length));
        env->vtbl->ReleaseStringChars(env, str, chars);
        return result;
    } else {
        std::wstring result((size_t)length, 0);
        for (jint i = 0; i < length; i++)
            result[i] = wchar_t(chars[i]);
        env->vtbl->ReleaseStringChars(env, str, chars);
        return result;
    }
}

std::filesystem::path JVM::toPath(jstring str)
{
    if constexpr (sizeof(std::filesystem::path::value_type) == sizeof(char))
        return toUtf8(str);
    else
        return toWString(str);
}

jstring JVM::className(jobject obj)
{
    if (!obj)
        return nullptr;

    jclass objectClass = env->vtbl->GetObjectClass(env, obj);
    if (!objectClass)
        return nullptr;

    jmethodID getClassMethod = env->vtbl->GetMethodID(env, objectClass, "getClass", "()Ljava/lang/Class;");
    env->vtbl->DeleteLocalRef(env, objectClass);
    if (!getClassMethod)
        return nullptr;

    jobject classObject = env->vtbl->CallObjectMethod(env, obj, getClassMethod);
    if (!classObject)
        return nullptr;

    jclass classClass = env->vtbl->GetObjectClass(env, classObject);
    if (!classClass) {
        env->vtbl->DeleteLocalRef(env, classObject);
        return nullptr;
    }

    jmethodID getNameMethod = env->vtbl->GetMethodID(env, classClass, "getName", "()Ljava/lang/String;");
    env->vtbl->DeleteLocalRef(env, classClass);
    if (!getNameMethod) {
        env->vtbl->DeleteLocalRef(env, classObject);
        return nullptr;
    }

    jstring name = (jstring)env->vtbl->CallObjectMethod(env, classObject, getNameMethod);
    env->vtbl->DeleteLocalRef(env, classObject);

    return name;
}

jclass JVM::stringClass()
{
    if (!stringClassRef) {
        stringClassRef = env->vtbl->FindClass(env, "java/lang/String");
        if (stringClassRef)
            stringClassRef = env->vtbl->NewGlobalRef(env, stringClassRef);
    }
    return stringClassRef;
}

bool JVM::compile(const JStringList& args)
{
    jclass compilerClass = env->vtbl->FindClass(env, "com/sun/tools/javac/Main");
    if (!compilerClass)
        return false;

    jmethodID compileMethodID = env->vtbl->GetStaticMethodID(env, compilerClass, "compile", "([Ljava/lang/String;)I");
    if (!compileMethodID) {
        env->vtbl->DeleteLocalRef(env, compilerClass);
        return false;
    }

    jobjectArray argList = args.toJavaArray();
    if (!argList) {
        env->vtbl->DeleteLocalRef(env, compilerClass);
        return false;
    }

    jint result = env->vtbl->CallStaticIntMethod(env, compilerClass, compileMethodID, argList);

    env->vtbl->DeleteLocalRef(env, argList);
    env->vtbl->DeleteLocalRef(env, compilerClass);

    return (!env->vtbl->ExceptionCheck(env) && result == 0);
}
