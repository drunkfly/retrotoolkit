#include "JNIStringRef.h"
#include "Compiler/Java/JVM.h"

JNIStringRef JNIStringRef::from(const char* utf8)
{
    auto env = JVM::jniEnv();
    return env->vtbl->NewStringUTF(env, utf8);
}

JNIStringRef JNIStringRef::from(const std::string& utf8)
{
    auto env = JVM::jniEnv();
    return env->vtbl->NewStringUTF(env, utf8.c_str());
}

JNIStringRef JNIStringRef::from(const std::wstring& str)
{
    auto env = JVM::jniEnv();
    size_t n = str.length();
    if constexpr (sizeof(jchar) == sizeof(wchar_t))
        return env->vtbl->NewString(env, reinterpret_cast<const jchar*>(str.c_str()), jint(n));
    else {
        std::unique_ptr<jchar[]> buf{new (std::nothrow) jchar[n]};
        if (!buf.get())
            return nullptr;
        for (size_t i = 0; i < n; i++)
            buf[i] = str[i];
        return env->vtbl->NewString(env, buf.get(), n);
    }
}

JNIStringRef JNIStringRef::from(const std::filesystem::path& str)
{
    return from(str.native());
}

std::string JNIStringRef::toUtf8() const
{
    std::string result;

    if (!mObjectRef)
        return result;

    auto env = JVM::jniEnv();
    jint length = env->vtbl->GetStringUTFLength(env, mObjectRef);
    if (length <= 0)
        return result;

    const char* utf = env->vtbl->GetStringUTFChars(env, mObjectRef, nullptr);
    if (!utf)
        return result;

    try {
        result.assign(utf, size_t(length));
    } catch (...) {
        env->vtbl->ReleaseStringUTFChars(env, mObjectRef, utf);
        throw;
    }

    env->vtbl->ReleaseStringUTFChars(env, mObjectRef, utf);
    return result;
}

std::wstring JNIStringRef::toWString() const
{
    std::wstring result;

    if (!mObjectRef)
        return result;

    auto env = JVM::jniEnv();
    jint length = env->vtbl->GetStringLength(env, mObjectRef);
    if (length <= 0)
        return result;

    const jchar* chars = env->vtbl->GetStringChars(env, mObjectRef, nullptr);
    if (!chars)
        return result;

    try {
        if constexpr (sizeof(jchar) == sizeof(wchar_t))
            result.assign(reinterpret_cast<const wchar_t*>(chars), size_t(length));
        else {
            result.assign(size_t(length), 0);
            for (jint i = 0; i < length; i++)
                result[i] = wchar_t(chars[i]);
        }
    } catch (...) {
        env->vtbl->ReleaseStringChars(env, mObjectRef, chars);
        throw;
    }

    env->vtbl->ReleaseStringChars(env, mObjectRef, chars);
    return result;
}

std::filesystem::path JNIStringRef::toPath() const
{
    if constexpr (sizeof(std::filesystem::path::value_type) == sizeof(char))
        return toUtf8();
    else
        return toWString();
}
