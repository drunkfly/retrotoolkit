#include "JNIClassRef.h"
#include "Compiler/CompilerError.h"
#include "Compiler/Java/JNIStringRef.h"
#include "Compiler/Java/JNIThrowableRef.h"
#include "Compiler/Java/JVM.h"

void JNIClassRef::loadGlobal(const char* name)
{
    if (mObjectRef)
        return;

    auto env = JVM::jniEnv();
    JNIClassRef localRef = env->vtbl->FindClass(env, name);
    if (localRef)
        mObjectRef = env->vtbl->NewGlobalRef(env, localRef.toJNI());
    if (!mObjectRef) {
        JNIThrowableRef::rethrowCurrentException();
        std::stringstream ss;
        ss << "Unable to resolve class \"" << name << "\".";
        throw CompilerError(nullptr, ss.str());
    }

    mName = name;
    for (char& ch : mName) {
        if (ch == '/')
            ch = '.';
    }
}

void JNIClassRef::defineGlobal(const char* name, const void* data, size_t size)
{
    if (mObjectRef)
        return;

    auto env = JVM::jniEnv();
    auto p = reinterpret_cast<const jbyte*>(data);
    JNIClassRef localRef = env->vtbl->DefineClass(env, name, nullptr, p, jint(size));
    if (localRef)
        mObjectRef = env->vtbl->NewGlobalRef(env, localRef.toJNI());
    if (!mObjectRef) {
        JNIThrowableRef::rethrowCurrentException();
        std::stringstream ss;
        ss << "Unable to define class \"" << name << "\".";
        throw CompilerError(nullptr, ss.str());
    }

    mName = name;
    for (char& ch : mName) {
        if (ch == '/')
            ch = '.';
    }
}

void JNIClassRef::release()
{
    auto env = JVM::jniEnv();

    if (mNativeMethodsRegistered) {
        env->vtbl->UnregisterNatives(env, mObjectRef);
        mNativeMethodsRegistered = false;
    }

    JNIRef::release();

    mName.clear();
}

const std::string& JNIClassRef::name()
{
    ensureNameResolved();
    return mName;
}

void JNIClassRef::registerNatives(jint count, const JNINativeMethod* methods)
{
    if (mNativeMethodsRegistered)
        return;

    if (!mObjectRef)
        throw CompilerError(nullptr, "Unable to register native methods for NULL class.");

    auto env = JVM::jniEnv();
    jint r = env->vtbl->RegisterNatives(env, mObjectRef, methods, count);
    if (r != JNI_OK) {
        std::stringstream ss;
        ss << "Unable to register native methods for class \"" << name() << "\": ";
        JVM::printJniError(ss, r);
        throw CompilerError(nullptr, ss.str());
    }

    mNativeMethodsRegistered = true;
}

void JNIClassRef::resolveMethod(jmethodID& method, const char* name, const char* signature)
{
    if (!method)
        method = resolveMethod(name, signature);
}

jmethodID JNIClassRef::resolveMethod(const char* name, const char* signature)
{
    auto env = JVM::jniEnv();
    jmethodID method = env->vtbl->GetMethodID(env, mObjectRef, name, signature);
    if (!method) {
        JNIThrowableRef::rethrowCurrentException();
        std::stringstream ss;
        ss << "Unable to resolve method \"" << name << signature << "\"  of class \"" << this->name() << "\".";
        throw CompilerError(nullptr, ss.str());
    }
    return method;
}

void JNIClassRef::resolveStaticMethod(jmethodID& method, const char* name, const char* signature)
{
    if (!method)
        method = resolveStaticMethod(name, signature);
}

jmethodID JNIClassRef::resolveStaticMethod(const char* name, const char* signature)
{
    auto env = JVM::jniEnv();
    jmethodID method = env->vtbl->GetStaticMethodID(env, mObjectRef, name, signature);
    if (!method) {
        JNIThrowableRef::rethrowCurrentException();
        std::stringstream ss;
        ss << "Unable to resolve static method \"" << name << signature << "\" of class \"" << this->name() << "\".";
        throw CompilerError(nullptr, ss.str());
    }
    return method;
}

void JNIClassRef::constructGlobal(JNIRef& object, const char* signature, ...)
{
    if (object)
        return;

    auto env = JVM::jniEnv();
    jmethodID constructorID = resolveMethod("<init>", signature);

    va_list args;
    va_start(args, signature);
    JNIRef localRef = env->vtbl->NewObjectV(env, mObjectRef, constructorID, args);
    va_end(args);

    object = localRef.newGlobalRef();
    if (!object) {
        JNIThrowableRef::rethrowCurrentException();
        std::stringstream ss;
        ss << "Unable to construct instance of class \"" << name() << "\".";
        throw CompilerError(nullptr, ss.str());
    }
}

void JNIClassRef::ensureNameResolved()
{
    if (!mName.empty() || !mObjectRef || !mAllowNameResolve)
        return;

    struct Disable {
        bool* mFlag;
        explicit Disable(bool* flag) : mFlag(flag) { *mFlag = false; }
        ~Disable() { *mFlag = true; }
    };

    Disable disable(&mAllowNameResolve);

    auto env = JVM::jniEnv();
    JNIClassRef classClass = env->vtbl->GetObjectClass(env, mObjectRef);
    if (!classClass)
        return;

    jmethodID getNameMethod = classClass.resolveMethod("getName", "()Ljava/lang/String;");
    if (!getNameMethod)
        return;

    JNIStringRef name = env->vtbl->CallObjectMethod(env, mObjectRef, getNameMethod);
    if (!name)
        return;

    mName = name.toUtf8();
}
