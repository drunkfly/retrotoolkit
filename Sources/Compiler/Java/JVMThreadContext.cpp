#include "JVMThreadContext.h"
#include "Compiler/Java/JVMGlobalContext.h"
#include "Compiler/Java/JNIClassRef.h"
#include "Compiler/Java/JStringList.h"
#include "Compiler/CompilerError.h"

static thread_local JVMThreadContext* instance;

JVMThreadContext::JVMThreadContext()
    : mCompilerListener(nullptr)
{
    if (::instance)
        throw CompilerError(nullptr, "Internal compiler error: multiple instances of JVMThreadContext.");

    ::instance = this;
}

JVMThreadContext::~JVMThreadContext()
{
    if (::instance == this)
        ::instance = nullptr;

    releaseAll();
}

bool JVMThreadContext::hasInstance()
{
    return ::instance != nullptr;
}

JVMThreadContext* JVMThreadContext::instance()
{
    if (!::instance)
        throw CompilerError(nullptr, "Internal compiler error: JVMThreadContext is not available.");

    return ::instance;
}

ICompilerListener* JVMThreadContext::listener() const
{
    if (!mCompilerListener)
        throw CompilerError(nullptr, "Internal compiler error: JVMThreadContext is not available.");

    return mCompilerListener;
}

void JVMThreadContext::ensureInitialized()
{
}

void JVMThreadContext::releaseAll()
{
    releaseClassLoader();
}

bool JVMThreadContext::constructClassLoader(const JStringList* classPath)
{
    JNIRef classPathArray;
    if (classPath) {
        classPathArray = classPath->toJavaArray();
        if (!classPathArray)
            return false;
    }

    mClassLoader = JVMGlobalContext::instance()->constructClassLoader(classPathArray);
    return !!mClassLoader;
}

void JVMThreadContext::releaseClassLoader()
{
    mClassLoader.release();
}

const JNIRef& JVMThreadContext::classLoader() const
{
    if (!mClassLoader)
        throw CompilerError(nullptr, "Internal compiler error: NULL class loader.");

    return mClassLoader;
}

JNIClassRef JVMThreadContext::loadWithClassLoader(const JNIStringRef& className)
{
    if (!mClassLoader)
        throw CompilerError(nullptr, "Internal compiler error: NULL class loader.");

    return JVMGlobalContext::instance()->invokeClassLoader(mClassLoader, className);
}
