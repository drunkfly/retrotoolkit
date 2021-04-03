#include "Exceptions.h"
#include "Compiler/Java/JNIStringRef.h"
#include "Compiler/Java/JavaStdClasses.h"
#include "Compiler/Java/JVM.h"

void javaThrow()
{
    javaThrow(JavaStdClasses::java_lang_RuntimeException, "C++ exception", nullptr);
}

void javaThrow(const std::bad_alloc& e)
{
    javaThrow(JavaStdClasses::java_lang_OutOfMemoryError, nullptr, e.what());
}

void javaThrow(const std::exception& e)
{
    javaThrow(JavaStdClasses::java_lang_RuntimeException, "C++ exception", e.what());
}

void javaThrow(const CompilerError& e)
{
    JTRY
        std::stringstream ss;
        CompilerError::locationToString(e.location(), "", ss);
        javaThrow(JavaStdClasses::java_lang_RuntimeException, ss.str().c_str(), e.message().c_str());
    JCATCH
}

void javaThrow(const JNIClassRef& exceptionClass, const char* prefix, const char* message)
{
    auto env = JVM::jniEnv();
    std::string msg;

    try {
        std::stringstream ss;
        if (prefix && *prefix) {
            ss << prefix;
            if (message && *message)
                ss << ": ";
        }
        ss << message;
        msg = ss.str();
    } catch (...) {
    }

    env->vtbl->ThrowNew(env, exceptionClass.toJNI(), msg.c_str());
}
