#include "JStringList.h"
#include "Compiler/Java/JNIStringRef.h"
#include "Compiler/Java/JavaStdClasses.h"
#include "Compiler/Java/JVM.h"

JStringList::JStringList()
{
}

JStringList::~JStringList()
{
}

void JStringList::clear()
{
    mArguments.clear();
}

void JStringList::reserve(size_t size)
{
    mArguments.reserve(size);
}

void JStringList::add(const char* str)
{
    Argument arg;
    arg.string = str;
    mArguments.emplace_back(std::move(arg));
}

void JStringList::add(const wchar_t* str)
{
    Argument arg;
    arg.wstring = str;
    mArguments.emplace_back(std::move(arg));
}

void JStringList::add(std::string str)
{
    Argument arg;
    arg.string = std::move(str);
    mArguments.emplace_back(std::move(arg));
}

void JStringList::add(std::wstring wstr)
{
    Argument arg;
    arg.wstring = std::move(wstr);
    mArguments.emplace_back(std::move(arg));
}

void JStringList::add(std::filesystem::path path)
{
    Argument arg;
    arg.path = std::move(path);
    mArguments.emplace_back(std::move(arg));
}

JNIRef JStringList::toJavaArray() const
{
    auto env = JVM::jniEnv();

    JNIRef arr = env->vtbl->NewObjectArray(env,
        jint(mArguments.size()), JavaStdClasses::java_lang_String.toJNI(), nullptr);
    if (!arr)
        return arr;

    int index = 0;
    for (const auto& arg : mArguments) {
        JNIStringRef str;
        if (arg.string.has_value())
            str = JNIStringRef::from(*arg.string);
        else if (arg.wstring.has_value())
            str = JNIStringRef::from(*arg.wstring);
        else if (arg.path.has_value())
            str = JNIStringRef::from(*arg.path);

        if (!str) {
            arr.release();
            return arr;
        }

        env->vtbl->SetObjectArrayElement(env, arr.toJNI(), index, str.toJNI());
        ++index;
    }

    return arr;
}
