#ifndef COMPILER_JAVA_JNICLASS_H
#define COMPILER_JAVA_JNICLASS_H

#include "Compiler/Java/JNIRef.h"

class JNIClassRef final : public JNIRef
{
    JNIREF(JNIClassRef, jclass)

public:
    void loadGlobal(const char* name);
    void defineGlobal(const char* name, const void* data, size_t size);
    void release() override;

    const std::string& name();

    void registerNatives(jint count, const JNINativeMethod* methods);
    template <size_t N> void registerNatives(const JNINativeMethod (&&m)[N]) { registerNatives(jint(N), m); }

    void resolveMethod(jmethodID& method, const char* name, const char* signature);
    jmethodID resolveMethod(const char* name, const char* signature);

    void resolveStaticMethod(jmethodID& method, const char* name, const char* signature);
    jmethodID resolveStaticMethod(const char* name, const char* signature);

    void constructGlobal(JNIRef& object, const char* signature, ...);

private:
    std::string mName;
    bool mNativeMethodsRegistered = false;
    bool mAllowNameResolve = true;

    void ensureNameResolved();

    DISABLE_COPY(JNIClassRef);
};

#endif
