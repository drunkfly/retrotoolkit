#include "JVM.h"
#include "Compiler/Java/JStringList.h"
#include "Compiler/Compiler.h"
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

extern const unsigned JavaOutputWriter_len;
extern const unsigned char JavaOutputWriter[];
extern const unsigned JavaBuilder_len;
extern const unsigned char JavaBuilder[];
extern const unsigned JavaBuilderClassLoader_len;
extern const unsigned char JavaBuilderClassLoader[];
extern const unsigned JavaBuilderLauncher_len;
extern const unsigned char JavaBuilderLauncher[];

static JavaVM* jvm;
static JNIEnv* env;
static std::filesystem::path loadedDllPath;
static jclass stringClassRef;
static jclass outputWriterClassRef;
static bool outputWriterMethodsRegistered;
static jobject outputWriterRef;
static jclass printWriterClassRef;
static jobject printWriterRef;
static jclass classLoaderClassRef;
static bool classLoaderMethodsRegistered;
static jmethodID classLoaderConstructorID;
static jmethodID classLoaderLoadClassMethodID;
static jclass compilerClassRef;
static jmethodID compilerMethodID;
static jclass builderLauncherClassRef;
static jclass builderClassRef;
static ICompilerListener* compilerListener;
static jobject classLoader;

static void printJniError(std::stringstream& ss, int r)
{
    switch (r) {
        case JNI_ERR: ss << "JNI_ERR"; break;
        case JNI_EDETACHED: ss << "JNI_EDETACHED"; break;
        case JNI_EVERSION: ss << "JNI_EVERSION"; break;
        case JNI_ENOMEM: ss << "JNI_ENOMEM"; break;
        case JNI_EEXIST: ss << "JNI_EEXIST"; break;
        case JNI_EINVAL: ss << "JNI_EINVAL"; break;
        default: ss << "code " << r;
    }
}

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
    javacPath = jdkPath / "bin/javac";
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
            printJniError(ss, r);
            throw CompilerError(nullptr, ss.str());
        }
    }

    ensureNecessaryClassesLoaded();
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
    outputWriterClassRef = nullptr;
    outputWriterMethodsRegistered = false;
    outputWriterRef = nullptr;
    printWriterClassRef = nullptr;
    printWriterRef = nullptr;
    classLoaderClassRef = nullptr;
    classLoaderMethodsRegistered = false;
    classLoaderConstructorID = nullptr;
    classLoaderLoadClassMethodID = nullptr;
    compilerClassRef = nullptr;
    compilerMethodID = nullptr;
    builderLauncherClassRef = nullptr;
    builderClassRef = nullptr;
}

bool JVM::isAttached()
{
    jint r = jvm->vtbl->GetEnv(jvm, &env, JNI_VERSION_1_4);
    if (r != JNI_OK && r != JNI_EDETACHED) {
        std::stringstream ss;
        ss << "Unable to retrieve Java Virtual Machine environment: ";
        printJniError(ss, r);
        throw CompilerError(nullptr, ss.str());
    }
    return (r == JNI_OK);
}

void JVM::attachCurrentThread()
{
    jint r = jvm->vtbl->AttachCurrentThread(jvm, &env, nullptr);
    if (r != JNI_OK) {
        std::stringstream ss;
        ss << "Unable to attach thread to Java Virtual Machine: ";
        printJniError(ss, r);
        throw CompilerError(nullptr, ss.str());
    }

    try {
        ensureNecessaryClassesLoaded();
    } catch (...) {
        detachCurrentThread();
        throw;
    }
}

void JVM::detachCurrentThread()
{
    jint r = jvm->vtbl->DetachCurrentThread(jvm);
    if (r != JNI_OK) {
        std::stringstream ss;
        ss << "Unable to detach thread from Java Virtual Machine: ";
        printJniError(ss, r);
        throw CompilerError(nullptr, ss.str());
    }
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

jobject JVM::makeGlobalRef(jobject local)
{
    if (!local)
        return nullptr;

    jobject global = env->vtbl->NewGlobalRef(env, local);
    env->vtbl->DeleteLocalRef(env, local);

    return global;
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
    return stringClassRef;
}

bool JVM::compile(const JStringList& args, ICompilerListener* listener)
{
    assert(!compilerListener);

    jobjectArray argList = args.toJavaArray();
    if (!argList)
        return false;

    compilerListener = listener;
    jint result = env->vtbl->CallStaticIntMethod(env, compilerClassRef, compilerMethodID, argList, printWriterRef);
    compilerListener = nullptr;

    env->vtbl->DeleteLocalRef(env, argList);

    return (!env->vtbl->ExceptionCheck(env) && result == 0);
}

bool JVM::runClass(const char* className, const JStringList& args,
    ICompilerListener* listener, bool useClassLoader, const JStringList* classPath)
{
    assert(!compilerListener);
    assert(!classLoader);

    if (useClassLoader) {
        jobjectArray classPathArray = nullptr;
        if (classPath) {
            classPathArray = classPath->toJavaArray();
            if (!classPathArray)
                return false;
        }

        compilerListener = listener;
        classLoader = env->vtbl->NewObject(env, classLoaderClassRef, classLoaderConstructorID, classPathArray);
        compilerListener = nullptr;

        if (classPathArray)
            env->vtbl->DeleteLocalRef(env, classPathArray);

        if (!classLoader)
            return false;
    }

    jclass classRef;
    if (!strcmp(className, "drunkfly/BuilderLauncher"))
        classRef = builderLauncherClassRef;
    else if (!classLoader)
        classRef = env->vtbl->FindClass(env, className);
    else {
        jstring classNameString = env->vtbl->NewStringUTF(env, className);
        if (!classNameString) {
            env->vtbl->DeleteLocalRef(env, classLoader);
            classLoader = nullptr;
            return false;
        }

        compilerListener = listener;
        classRef = env->vtbl->CallObjectMethod(env, classLoader, classLoaderLoadClassMethodID, classNameString);
        compilerListener = nullptr;

        env->vtbl->DeleteLocalRef(env, classNameString);
    }

    if (!classRef) {
        if (classLoader) {
            env->vtbl->DeleteLocalRef(env, classLoader);
            classLoader = nullptr;
        }
        return false;
    }

    jmethodID mainMethodID = env->vtbl->GetStaticMethodID(env, classRef, "main", "([Ljava/lang/String;)V");
    if (!mainMethodID) {
        if (classRef != builderLauncherClassRef)
            env->vtbl->DeleteLocalRef(env, classRef);
        if (classLoader) {
            env->vtbl->DeleteLocalRef(env, classLoader);
            classLoader = nullptr;
        }
        return false;
    }

    jobjectArray argList = args.toJavaArray();
    if (!argList) {
        if (classRef != builderLauncherClassRef)
            env->vtbl->DeleteLocalRef(env, classRef);
        if (classLoader) {
            env->vtbl->DeleteLocalRef(env, classLoader);
            classLoader = nullptr;
        }
        return false;
    }

    compilerListener = listener;
    env->vtbl->CallStaticVoidMethod(env, classRef, mainMethodID, argList);
    compilerListener = nullptr;

    env->vtbl->DeleteLocalRef(env, argList);
    if (classRef != builderLauncherClassRef)
        env->vtbl->DeleteLocalRef(env, classRef);
    if (classLoader) {
        env->vtbl->DeleteLocalRef(env, classLoader);
        classLoader = nullptr;
    }

    return !env->vtbl->ExceptionCheck(env);
}

void JNICALL JVM::drunkfly_Messages_print(JNIEnv* env, jclass, jstring message)
{
    assert(compilerListener);
    if (compilerListener)
        compilerListener->printMessage(toUtf8(message));
}

jobject JNICALL JVM::drunkfly_Messages_getInstance(JNIEnv* env, jclass)
{
    return outputWriterRef;
}

jobject JNICALL JVM::drunkfly_Messages_getPrintWriter(JNIEnv* env, jclass)
{
    return printWriterRef;
}

jobject JNICALL JVM::drunkfly_BuilderClassLoader_getInstance(JNIEnv* env, jclass)
{
    assert(classLoader);
    return classLoader;
}

void JVM::ensureNecessaryClassesLoaded()
{
    if (!stringClassRef) {
        stringClassRef = makeGlobalRef(env->vtbl->FindClass(env, "java/lang/String"));
        if (!stringClassRef) {
            throwIfException();
            throw CompilerError(nullptr, "Unable to resolve class \"java.lang.String\".");
        }
    }

    if (!outputWriterClassRef) {
        auto p = reinterpret_cast<const jbyte*>(JavaOutputWriter);
        outputWriterClassRef =
            makeGlobalRef(env->vtbl->DefineClass(env, "drunkfly/Messages", nullptr, p, JavaOutputWriter_len));
        if (!outputWriterClassRef) {
            throwIfException();
            throw CompilerError(nullptr, "Unable to resolve class \"drunkfly.Messages\".");
        }
    }

    if (!outputWriterMethodsRegistered) {
        const JNINativeMethod methods[] = {
                { "print", "(Ljava/lang/String;)V", drunkfly_Messages_print },
                { "getInstance", "()Ldrunkfly/Messages;", drunkfly_Messages_getInstance },
                { "getPrintWriter", "()Ljava/io/PrintWriter;", drunkfly_Messages_getPrintWriter },
            };
        jint r = env->vtbl->RegisterNatives(env, outputWriterClassRef, methods, sizeof(methods) / sizeof(methods[0]));
        if (r != JNI_OK) {
            std::stringstream ss;
            ss << "Unable to register native methods for class \"drunkfly.Messages\": ";
            printJniError(ss, r);
            throw CompilerError(nullptr, ss.str());
        }

        outputWriterMethodsRegistered = true;
    }

    if (!outputWriterRef) {
        jmethodID constructorID = env->vtbl->GetMethodID(env, outputWriterClassRef, "<init>", "()V");
        if (!constructorID) {
            throwIfException();
            throw CompilerError(nullptr, "Unable to resolve constructor of class \"drunkfly.Messages\".");
        }

        outputWriterRef = makeGlobalRef(env->vtbl->NewObject(env, outputWriterClassRef, constructorID));
        if (!outputWriterRef) {
            throwIfException();
            throw CompilerError(nullptr, "Unable to construct instance of class \"drunkfly.Messages\".");
        }
    }

    if (!printWriterClassRef) {
        printWriterClassRef = makeGlobalRef(env->vtbl->FindClass(env, "java/io/PrintWriter"));
        if (!printWriterClassRef) {
            throwIfException();
            throw CompilerError(nullptr, "Unable to resolve class \"java.io.PrintWriter\".");
        }
    }

    if (!printWriterRef) {
        jmethodID constructorID = env->vtbl->GetMethodID(env, printWriterClassRef, "<init>", "(Ljava/io/Writer;)V");
        if (!constructorID) {
            throwIfException();
            throw CompilerError(nullptr, "Unable to resolve constructor of class \"java.io.PrintWriter\".");
        }

        printWriterRef = makeGlobalRef(env->vtbl->NewObject(env, printWriterClassRef, constructorID, outputWriterRef));
        if (!printWriterRef) {
            throwIfException();
            throw CompilerError(nullptr, "Unable to construct instance of class \"java.io.PrintWriter\".");
        }
    }

    if (!classLoaderClassRef) {
        auto p = reinterpret_cast<const jbyte*>(JavaBuilderClassLoader);
        classLoaderClassRef = makeGlobalRef(
            env->vtbl->DefineClass(env, "drunkfly/BuilderClassLoader", nullptr, p, JavaBuilderClassLoader_len));
        if (!classLoaderClassRef) {
            throwIfException();
            throw CompilerError(nullptr, "Unable to resolve class \"drunkfly.BuilderClassLoader\".");
        }
    }

    if (!classLoaderMethodsRegistered) {
        const JNINativeMethod method =
            { "getInstance", "()Ldrunkfly/BuilderClassLoader;", drunkfly_BuilderClassLoader_getInstance };
        jint r = env->vtbl->RegisterNatives(env, classLoaderClassRef, &method, 1);
        if (r != JNI_OK) {
            std::stringstream ss;
            ss << "Unable to register native methods for class \"drunkfly.BuilderClassLoader\": ";
            printJniError(ss, r);
            throw CompilerError(nullptr, ss.str());
        }

        classLoaderMethodsRegistered = true;
    }

    if (!classLoaderConstructorID) {
        classLoaderConstructorID =
            env->vtbl->GetMethodID(env, classLoaderClassRef, "<init>", "([Ljava/lang/String;)V");
        if (!classLoaderConstructorID) {
            throwIfException();
            throw CompilerError(nullptr, "Unable to resolve constructor of class \"drunkfly.BuilderClassLoader\".");
        }
    }

    if (!classLoaderLoadClassMethodID) {
        classLoaderLoadClassMethodID =
            env->vtbl->GetMethodID(env, classLoaderClassRef, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
        if (!classLoaderLoadClassMethodID) {
            throwIfException();
            throw CompilerError(nullptr,
                "Unable to resolve method \"loadClass\" of class \"drunkfly.BuilderClassLoader\".");
        }
    }

    if (!compilerClassRef) {
        compilerClassRef = makeGlobalRef(env->vtbl->FindClass(env, "com/sun/tools/javac/Main"));
        if (!compilerClassRef) {
            throwIfException();
            throw CompilerError(nullptr, "Unable to resolve class \"com.sun.tools.javac.Main\".");
        }
    }

    if (!compilerMethodID) {
        compilerMethodID = env->vtbl->GetStaticMethodID(env,
            compilerClassRef, "compile", "([Ljava/lang/String;Ljava/io/PrintWriter;)I");
        if (!compilerMethodID) {
            throwIfException();
            throw CompilerError(nullptr, "Unable to find method \"compile\" in class \"com.sun.tools.javac.Main\".");
        }
    }

    if (!builderLauncherClassRef) {
        auto p = reinterpret_cast<const jbyte*>(JavaBuilderLauncher);
        builderLauncherClassRef = makeGlobalRef(
            env->vtbl->DefineClass(env, "drunkfly/BuilderLauncher", nullptr, p, JavaBuilderLauncher_len));
        if (!builderLauncherClassRef) {
            throwIfException();
            throw CompilerError(nullptr, "Unable to resolve class \"drunkfly.BuilderLauncher\".");
        }
    }

    if (!builderClassRef) {
        auto p = reinterpret_cast<const jbyte*>(JavaBuilder);
        builderClassRef = makeGlobalRef(env->vtbl->DefineClass(env, "drunkfly/Builder", nullptr, p, JavaBuilder_len));
        if (!builderClassRef) {
            throwIfException();
            throw CompilerError(nullptr, "Unable to resolve class \"drunkfly.Builder\".");
        }
    }
}
