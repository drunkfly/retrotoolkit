#include "JVMGlobalContext.h"
#include "Compiler/Java/Natives/drunkfly_Messages.h"
#include "Compiler/Java/Natives/drunkfly_internal_BuilderClassLoader.h"
#include "Compiler/Java/JNIStringRef.h"
#include "Compiler/Java/JavaClasses.h"
#include "Compiler/Java/JVMThreadContext.h"
#include "Compiler/Java/JVM.h"
#include "Compiler/CompilerError.h"

JVMGlobalContext* instance;

JVMGlobalContext::JVMGlobalContext()
    : mClassLoaderConstructorID(nullptr)
    , mClassLoaderLoadClassMethodID(nullptr)
    , mCompilerMethodID(nullptr)
    , mWasInitialized(false)
{
    if (::instance)
        throw CompilerError(nullptr, "Internal compiler error: multiple instances of JVMGlobalContext.");

    ::instance = this;
}

JVMGlobalContext::~JVMGlobalContext()
{
    if (mWasInitialized) {
        JVM::AttachCurrentThread attach;
        releaseAll();
    }

    if (::instance == this)
        ::instance = nullptr;
}

bool JVMGlobalContext::hasInstance()
{
    return ::instance != nullptr;
}

JVMGlobalContext* JVMGlobalContext::instance()
{
    if (!::instance)
        throw CompilerError(nullptr, "Internal compiler error: JVMGlobalContext is not available.");

    return ::instance;
}

void JVMGlobalContext::ensureInitialized()
{
    mWasInitialized = true;
    JavaClasses::ensureLoaded();

    JavaClasses::drunkfly_Messages.registerNatives({
            { "getInstance", "()Ldrunkfly/Messages;", drunkfly_Messages_getInstance },
            { "getPrintWriter", "()Ljava/io/PrintWriter;", drunkfly_Messages_getPrintWriter },
            { "print", "(Ljava/lang/String;)V", drunkfly_Messages_print },
        });

    JavaClasses::drunkfly_internal_BuilderClassLoader.registerNatives({
            { "getInstance", "()Ldrunkfly/internal/BuilderClassLoader;", drunkfly_internal_BuilderClassLoader_getInstance },
        });

    JavaClasses::com_sun_tools_javac_Main.resolveStaticMethod(
        mCompilerMethodID, "compile", "([Ljava/lang/String;Ljava/io/PrintWriter;)I");
    JavaClasses::drunkfly_internal_BuilderClassLoader.resolveMethod(
        mClassLoaderConstructorID, "<init>", "([Ljava/lang/String;)V");
    JavaClasses::drunkfly_internal_BuilderClassLoader.resolveMethod(
        mClassLoaderLoadClassMethodID, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    JavaClasses::drunkfly_Messages.constructGlobal(mOutputWriter, "()V");
    JavaClasses::java_io_PrintWriter.constructGlobal(mPrintWriter, "(Ljava/io/Writer;)V", mOutputWriter.toJNI());
}

void JVMGlobalContext::releaseAll()
{
    mPrintWriter.release();
    mOutputWriter.release();

    mClassLoaderConstructorID = nullptr;
    mClassLoaderLoadClassMethodID = nullptr;
    mCompilerMethodID = nullptr;

    JavaClasses::releaseAll();

    mWasInitialized = false;
}

const JNIRef& JVMGlobalContext::outputWriter() const
{
    if (!mOutputWriter)
        throw CompilerError(nullptr, "Internal compiler error: JVMGlobalContext was not properly initialized.");
    return mOutputWriter;
}

const JNIRef& JVMGlobalContext::printWriter() const
{
    if (!mPrintWriter)
        throw CompilerError(nullptr, "Internal compiler error: JVMGlobalContext was not properly initialized.");
    return mPrintWriter;
}

jint JVMGlobalContext::invokeJavaC(const JNIRef& argList) const
{
    if (!mCompilerMethodID || !mPrintWriter)
        throw CompilerError(nullptr, "Internal compiler error: JVMGlobalContext was not properly initialized.");

    auto env = JVM::jniEnv();
    return env->vtbl->CallStaticIntMethod(env,
        JavaStdClasses::com_sun_tools_javac_Main.toJNI(),
        mCompilerMethodID,
        argList.toJNI(),
        mPrintWriter.toJNI());
}

JNIRef JVMGlobalContext::constructClassLoader(const JNIRef& classPath) const
{
    if (!mClassLoaderConstructorID)
        throw CompilerError(nullptr, "Internal compiler error: JVMGlobalContext was not properly initialized.");

    auto env = JVM::jniEnv();
    return env->vtbl->NewObject(env,
        JavaClasses::drunkfly_internal_BuilderClassLoader.toJNI(),
        mClassLoaderConstructorID,
        classPath.toJNI());
}

JNIClassRef JVMGlobalContext::invokeClassLoader(const JNIRef& classLoader, const JNIStringRef& className) const
{
    if (!mClassLoaderLoadClassMethodID)
        throw CompilerError(nullptr, "Internal compiler error: JVMGlobalContext was not properly initialized.");

    if (!classLoader || !className)
        return nullptr;

    auto env = JVM::jniEnv();
    return env->vtbl->CallObjectMethod(env,
        classLoader.toJNI(),
        mClassLoaderLoadClassMethodID,
        className.toJNI());
}
