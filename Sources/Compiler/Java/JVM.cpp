#include "JVM.h"
#include "Compiler/Java/JNI.h"
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

bool JVM::isLoaded()
{
    return jvmDll || jvm || env;
}

const std::filesystem::path& JVM::loadedDllPath()
{
    return ::loadedDllPath;
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
  #endif

    // FIXME: Linux, MacOS

    std::stringstream ss;
    ss << "Unable to find jvm.dll in \"" << jdkPath.string() << "\".";
    throw CompilerError(nullptr, ss.str());
}

std::filesystem::path JVM::findJavaC(const std::filesystem::path& jdkPath)
{
    std::filesystem::path javacPath;

  #ifdef _WIN32
    javacPath = jdkPath / "bin/javac.exe";
    if (std::filesystem::exists(javacPath) && std::filesystem::is_regular_file(javacPath))
        return javacPath;
  #endif

    // FIXME: Linux, MacOS

    std::stringstream ss;
    ss << "Unable to find javac.exe in \"" << jdkPath.string() << "\".";
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
            throw CompilerError(nullptr, ss.str().c_str());
        }
      #else
        jvmDll = dlopen(dllPath.c_str(), RTLD_NOW | RTLD_GLOBAL);
        if (!jvmDll) {
            std::stringstream ss;
            ss << "Unable to load library \"" << dllPath.string() << "\": " << dlerror();
            throw CompilerError(nullptr, ss.str().c_str());
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
}
