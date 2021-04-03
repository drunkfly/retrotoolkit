#include "JVM.h"
#include "Common/IO.h"
#include "Compiler/Java/JNIClassRef.h"
#include "Compiler/Java/JNIStringRef.h"
#include "Compiler/Java/JStringList.h"
#include "Compiler/Java/JVMThreadContext.h"
#include "Compiler/Java/JVMGlobalContext.h"
#include "Compiler/Java/JavaClasses.h"
#include "Compiler/Compiler.h"
#include "Compiler/CompilerError.h"

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
static bool currentVerboseGC;
static bool currentVerboseClass;
static bool currentVerboseJNI;
static bool verboseGC;
static bool verboseClass;
static bool verboseJNI;

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

    // FIXME: MacOS

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

    // FIXME: MacOS

    std::stringstream ss;
    ss << "Unable to find Java compiler executable in \"" << jdkPath.string() << "\".";
    throw CompilerError(nullptr, ss.str());
}

std::filesystem::path JVM::findToolsJar(const std::filesystem::path& jdkPath)
{
    std::filesystem::path toolsJar;

    toolsJar = jdkPath / "lib/tools.jar";
    if (std::filesystem::exists(toolsJar) && std::filesystem::is_regular_file(toolsJar))
        return toolsJar;

    toolsJar = jdkPath / "jmods/jdk.compiler.jmod";
    if (std::filesystem::exists(toolsJar) && std::filesystem::is_regular_file(toolsJar))
        return toolsJar;

    std::stringstream ss;
    ss << "Unable to find \"tools.jar\" or \"jdk.compiler.jmod\" in \"" << jdkPath.string() << "\".";
    throw CompilerError(nullptr, ss.str());
}

void JVM::setVerboseGC(bool flag)
{
    verboseGC = flag;
}

void JVM::setVerboseClass(bool flag)
{
    verboseClass = flag;
}

void JVM::setVerboseJNI(bool flag)
{
    verboseJNI = flag;
}

bool JVM::loadedVerboseGC()
{
    return currentVerboseGC;
}

bool JVM::loadedVerboseClass()
{
    return currentVerboseClass;
}

bool JVM::loadedVerboseJNI()
{
    return currentVerboseJNI;
}

bool JVM::isLoaded()
{
    return jvmDll || jvm || env;
}

const std::filesystem::path& JVM::loadedDllPath()
{
    return ::loadedDllPath;
}

void JVM::load(const std::filesystem::path& jdkPath, const std::filesystem::path& classPath)
{
    if (!jvmDll) {
        std::filesystem::path dllPath = findJvmDll(jdkPath);
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
        const char* sep = ";";
        auto JNI_CreateJavaVM = (PFNJNICREATEJAVAVM)GetProcAddress(jvmDll, "JNI_CreateJavaVM");
        if (!JNI_CreateJavaVM) {
            DWORD dwLastError = GetLastError();
            unloadPermanently(); // unloading here allows user to change JVM path in settings and retry
            std::stringstream ss;
            ss << "Unable to resolve symbol \"JNI_CreateJavaVM\" in library \"" << ::loadedDllPath.string()
                << "\" (code 0x" << std::hex << std::setw(8) << std::setfill('0') << dwLastError << ").";
            throw CompilerError(nullptr, ss.str());
        }
      #else
        const char* sep = ":";
        dlerror();
        auto JNI_CreateJavaVM = (PFNJNICREATEJAVAVM)dlsym(jvmDll, "JNI_CreateJavaVM");
        if (!JNI_CreateJavaVM) {
            std::string error = dlerror();
            unloadPermanently(); // unloading here allows user to change JVM path in settings and retry
            std::stringstream ss;
            ss << "Unable to resolve symbol \"JNI_CreateJavaVM\" in library \""
                << ::loadedDllPath.string() << "\": " << error;
            throw CompilerError(nullptr, ss.str());
        }
      #endif

        std::stringstream ss;
        ss << "-Djava.class.path=";
        ss << findToolsJar(jdkPath).lexically_normal().string();
        ss << sep;
        ss << classPath.string();
        std::string classpath = ss.str();
      #ifdef _WIN32
        for (char& ch : classpath) {
            if (ch == '\\')
                ch = '/';
        }
      #endif

        std::vector<JavaVMOption> opts;
        opts.emplace_back(JavaVMOption{ "vfprintf", (void*)vfprintfHook });
        opts.emplace_back(JavaVMOption{ "exit", (void*)exitHook });
        opts.emplace_back(JavaVMOption{ "abort", (void*)abortHook });
        opts.emplace_back(JavaVMOption{ classpath.c_str() });
        if (verboseGC)
            opts.emplace_back(JavaVMOption{ "-verbose:gc" });
        if (verboseClass)
            opts.emplace_back(JavaVMOption{ "-verbose:class" });
        if (verboseJNI)
            opts.emplace_back(JavaVMOption{ "-verbose:jni" });

        currentVerboseGC = verboseGC;
        currentVerboseClass = verboseClass;
        currentVerboseJNI = verboseJNI;

        JavaVMInitArgs args;
        args.version = JNI_VERSION_1_4;
        args.options = opts.data();
        args.nOptions = int(opts.size());
        args.ignoreUnrecognized = JNI_FALSE;

        int r = JNI_CreateJavaVM(&jvm, &env, &args);
        if (r != JNI_OK) {
            std::stringstream ss;
            ss << "Unable to initialize Java Virtual Machine: ";
            printJniError(ss, r);
            throw CompilerError(nullptr, ss.str());
        }

        atexit(JVM::unloadPermanently);
    }

    JVMGlobalContext::instance()->ensureInitialized();
    JVMThreadContext::instance()->ensureInitialized();
}

void JVM::unloadPermanently()
{
    if (jvm) {
        jint r = jvm->vtbl->AttachCurrentThread(jvm, &env, nullptr);
        assert(r == JNI_OK);
        if (r == JNI_OK) {
            if (JVMThreadContext::hasInstance()) {
                try {
                    JVMThreadContext::instance()->releaseAll();
                } catch (...) {
                    assert(false);
                }
            }

            if (JVMGlobalContext::hasInstance()) {
                try {
                    JVMGlobalContext::instance()->releaseAll();
                } catch (...) {
                    assert(false);
                }
            }

            try {
                JavaClasses::releaseAll();
            } catch (...) {
                assert(false);
            }

            r = jvm->vtbl->DetachCurrentThread(jvm);
            assert(r == JNI_OK);
        }

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
        JVMGlobalContext::instance()->ensureInitialized();
        if (JVMThreadContext::hasInstance())
            JVMThreadContext::instance()->ensureInitialized();
    } catch (...) {
        detachCurrentThread();
        throw;
    }
}

void JVM::detachCurrentThread()
{
    if (!isLoaded())
        return;

    if (JVMThreadContext::hasInstance())
        JVMThreadContext::instance()->releaseAll();

    jint r = jvm->vtbl->DetachCurrentThread(jvm);
    if (r != JNI_OK) {
        std::stringstream ss;
        ss << "Unable to detach thread from Java Virtual Machine: ";
        printJniError(ss, r);
        throw CompilerError(nullptr, ss.str());
    }
}

void JVM::printJniError(std::stringstream& ss, int r)
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

bool JVM::compile(const JStringList& args)
{
    JNIRef argList = args.toJavaArray();
    if (!argList)
        return false;

    jint result = JVMGlobalContext::instance()->invokeJavaC(argList);
    return (!env->vtbl->ExceptionCheck(env) && result == 0);
}

bool JVM::runClass(const char* className, const JStringList& args, bool useClassLoader, const JStringList* classPath)
{
    struct ClassLoaderRAII {
        bool unload = false;
        ClassLoaderRAII() = default;
        ~ClassLoaderRAII() { if (unload) JVMThreadContext::instance()->releaseClassLoader(); }
        DISABLE_COPY(ClassLoaderRAII);
    };

    ClassLoaderRAII raii;

    auto threadContext = JVMThreadContext::instance();
    if (useClassLoader) {
        if (!threadContext->constructClassLoader(classPath))
            return false;
        raii.unload = true;
    }

    JNIClassRef classRef;
    if (JavaClasses::drunkfly_internal_BuilderLauncher.name() == className)
        classRef = JavaClasses::drunkfly_internal_BuilderLauncher.newLocalRef();
    else if (useClassLoader)
        classRef = threadContext->loadWithClassLoader(JNIStringRef::from(className));
    else
        classRef = env->vtbl->FindClass(env, className);
    if (!classRef)
        return false;

    jmethodID mainMethodID = classRef.resolveStaticMethod("main", "([Ljava/lang/String;)V");
    if (!mainMethodID)
        return false;

    JNIRef argList = args.toJavaArray();
    if (!argList)
        return false;

    env->vtbl->CallStaticVoidMethod(env, classRef.toJNI(), mainMethodID, argList.toJNI());
    return !env->vtbl->ExceptionCheck(env);
}

jint JVM::vfprintfHook(FILE* fp, const char* format, va_list args)
{
    char buf[1024];
    vsnprintf(buf, sizeof(buf), format, args);

    if (JVMThreadContext::hasInstance()) {
        try {
            auto listener = JVMThreadContext::instance()->listener();
            if (listener)
                listener->printMessage(buf);
        } catch (const std::exception& e) {
          #ifndef NDEBUG
           #ifdef _WIN32
            OutputDebugStringA(e.what());
            OutputDebugStringA("\n");
           #else
            fprintf(stderr, "%s\n", e.what());
           #endif
          #endif
            assert(false);
        } catch (...) {
            assert(false);
        }
    }

    return 0;
}

void JVM::exitHook(int code)
{
  #ifndef NDEBUG
   #ifdef _WIN32
    OutputDebugStringA("exitHook\n");
   #else
    fprintf(stderr, "exitHook\n");
   #endif
  #endif
    assert(false);
}

void JVM::abortHook()
{
  #ifndef NDEBUG
   #ifdef _WIN32
    OutputDebugStringA("abortHook\n");
   #else
    fprintf(stderr, "abortHook\n");
   #endif
  #endif
    assert(false);
}
