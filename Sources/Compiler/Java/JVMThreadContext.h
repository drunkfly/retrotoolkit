#ifndef COMPILER_JAVA_JVMTHREADCONTEXT_H
#define COMPILER_JAVA_JVMTHREADCONTEXT_H

#include "Compiler/Java/JNIRef.h"

class ICompilerListener;
class JStringList;
class JNIStringRef;
class JNIClassRef;

class JVMThreadContext
{
public:
    JVMThreadContext();
    ~JVMThreadContext();

    static bool hasInstance();
    static JVMThreadContext* instance();

    ICompilerListener* listener() const;
    void setListener(ICompilerListener* listener) { mCompilerListener = listener; }

    void ensureInitialized();
    void releaseAll();

    bool constructClassLoader(const JStringList* classPath);
    void releaseClassLoader();
    const JNIRef& classLoader() const;
    JNIClassRef loadWithClassLoader(const JNIStringRef& className);

private:
    ICompilerListener* mCompilerListener;
    JNIRef mClassLoader;

    DISABLE_COPY(JVMThreadContext);
};

#endif
