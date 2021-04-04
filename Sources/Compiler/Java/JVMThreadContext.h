#ifndef COMPILER_JAVA_JVMTHREADCONTEXT_H
#define COMPILER_JAVA_JVMTHREADCONTEXT_H

#include "Compiler/Java/JNIRef.h"

class GCHeap;
class SourceFile;
class ICompilerListener;
class JStringList;
class JNIStringRef;
class JNIClassRef;

class JVMThreadContext
{
public:
    explicit JVMThreadContext(GCHeap* heap);
    ~JVMThreadContext();

    static bool hasInstance();
    static JVMThreadContext* instance();

    ICompilerListener* listener() const;
    void setListener(ICompilerListener* listener) { mCompilerListener = listener; }

    void ensureInitialized();
    void releaseAll();

    const std::vector<SourceFile>& generatedFiles() const { return mGeneratedFiles; }
    void addGeneratedFile(const JNIStringRef& name, const JNIStringRef& path);

    bool constructClassLoader(const JStringList* classPath);
    void releaseClassLoader();
    const JNIRef& classLoader() const;
    JNIClassRef loadWithClassLoader(const JNIStringRef& className);

private:
    ICompilerListener* mCompilerListener;
    std::vector<SourceFile> mGeneratedFiles;
    GCHeap* mHeap;
    JNIRef mClassLoader;

    DISABLE_COPY(JVMThreadContext);
};

#endif
