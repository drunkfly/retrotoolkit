#include "JNIRef.h"
#include "Compiler/Java/JNIClassRef.h"
#include "Compiler/Java/JVM.h"

JNIRef::JNIRef(jobject object, Type type)
    : mObjectRef(object)
    , mType(type)
{
}

JNIRef::JNIRef(JNIRef&& other)
{
    moveFrom(other);
}

JNIRef::~JNIRef()
{
    release();
}

JNIRef& JNIRef::operator=(jobject object)
{
    release();
    mObjectRef = object;
    mType = Local;
    return *this;
}

JNIRef& JNIRef::operator=(JNIRef&& other)
{
    release();
    moveFrom(other);
    return *this;
}

void JNIRef::release()
{
    if (!mObjectRef)
        return;

    auto env = JVM::jniEnv();
    jobject ref = mObjectRef;
    mObjectRef = nullptr;

    switch (mType) {
        case Unknown:
            break;
        case Local:
            env->vtbl->DeleteLocalRef(env, ref);
            break;
        case Global:
            env->vtbl->DeleteGlobalRef(env, ref);
            break;
    }
}

JNIRef JNIRef::newGlobalRef() const
{
    if (!mObjectRef)
        return nullptr;

    auto env = JVM::jniEnv();
    return JNIRef(env->vtbl->NewGlobalRef(env, mObjectRef), Global);
}

JNIRef JNIRef::newLocalRef() const
{
    if (!mObjectRef)
        return nullptr;

    auto env = JVM::jniEnv();
    return JNIRef(env->vtbl->NewLocalRef(env, mObjectRef), Local);
}

JNIClassRef JNIRef::getClass() const
{
    if (!mObjectRef)
        return nullptr;

    auto env = JVM::jniEnv();
    return env->vtbl->GetObjectClass(env, mObjectRef);
}

void JNIRef::moveFrom(JNIRef& other)
{
    mObjectRef = other.mObjectRef;
    mType = other.mType;
    other.mObjectRef = nullptr;
}
