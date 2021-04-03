#include "JNIThrowableRef.h"
#include "Compiler/CompilerError.h"
#include "Compiler/Java/JVM.h"
#include "Compiler/Java/JNIThrowableRef.h"
#include "Compiler/Java/JNIClassRef.h"
#include "Compiler/Java/JNIStringRef.h"

JNIThrowableRef JNIThrowableRef::catchCurrent()
{
    auto env = JVM::jniEnv();
    if (!env->vtbl->ExceptionCheck(env))
        return nullptr;

    JNIThrowableRef throwable = env->vtbl->ExceptionOccurred(env);
    env->vtbl->ExceptionClear(env);

    return throwable;
}

void JNIThrowableRef::rethrow(SourceLocation* location) const
{
    if (!*this)
        throw CompilerError(location, "Unknown Java exception.");

    JNIClassRef throwableClass = getClass();
    if (!throwableClass)
        throw CompilerError(location, "Unknown Java exception.");

    std::string className = throwableClass.name();
    if (className.empty())
        throw CompilerError(location, "Unknown Java exception.");

    size_t index = className.rfind('.');
    if (index != std::string::npos)
        className = className.substr(index + 1);

    jmethodID throwableGetMessageMethod = throwableClass.resolveMethod("getMessage", "()Ljava/lang/String;");
    if (!throwableGetMessageMethod) {
        std::stringstream ss;
        ss << "JVM: " << className;
        throw CompilerError(location, ss.str());
    }

    auto env = JVM::jniEnv();
    JNIStringRef message = env->vtbl->CallObjectMethod(env, mObjectRef, throwableGetMessageMethod);
    if (!message) {
        std::stringstream ss;
        ss << "JVM: " << className;
        throw CompilerError(location, ss.str());
    }

    std::stringstream ss;
    ss << "JVM: " << className << ": " << message.toUtf8();
    throw CompilerError(location, ss.str());
}

void JNIThrowableRef::rethrowCurrentException(SourceLocation* location)
{
    auto env = JVM::jniEnv();
    if (!env->vtbl->ExceptionCheck(env))
        return;

    try {
        catchCurrent().rethrow(location);
    } catch (...) {
        env->vtbl->ExceptionClear(env);
        throw;
    }
}
