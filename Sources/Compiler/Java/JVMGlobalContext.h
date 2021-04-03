#ifndef COMPILER_JAVA_JVMGLOBALCONTEXT_H
#define COMPILER_JAVA_JVMGLOBALCONTEXT_H

#include "Compiler/Java/JNIRef.h"

class JNIStringRef;
class JNIClassRef;

class JVMGlobalContext
{
public:
    JVMGlobalContext();
    ~JVMGlobalContext();

    static bool hasInstance();
    static JVMGlobalContext* instance();

    void ensureInitialized();
    void releaseAll();

    const JNIRef& outputWriter() const;
    const JNIRef& printWriter() const;

    jint invokeJavaC(const JNIRef& argList) const;
    JNIRef constructClassLoader(const JNIRef& classPath) const;
    JNIClassRef invokeClassLoader(const JNIRef& classLoader, const JNIStringRef& className) const;

private:
    JNIRef mOutputWriter;
    JNIRef mPrintWriter;
    jmethodID mClassLoaderConstructorID;
    jmethodID mClassLoaderLoadClassMethodID;
    jmethodID mCompilerMethodID;
    bool mWasInitialized;

    DISABLE_COPY(JVMGlobalContext);
};

#endif
