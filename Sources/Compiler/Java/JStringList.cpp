#include "JStringList.h"
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

jobjectArray JStringList::toJavaArray() const
{
    auto env = JVM::jniEnv();

    auto arr = (jobjectArray)env->vtbl->NewObjectArray(env, jint(mArguments.size()), JVM::stringClass(), nullptr);
    if (!arr)
        return nullptr;

    int index = 0;
    for (const auto& arg : mArguments) {
        jstring str = nullptr;
        if (arg.string.has_value())
            str = JVM::toJString(*arg.string);
        else if (arg.wstring.has_value())
            str = JVM::toJString(*arg.wstring);
        else if (arg.path.has_value())
            str = JVM::toJString(*arg.path);

        if (!str) {
            env->vtbl->DeleteLocalRef(env, arr);
            return nullptr;
        }

        env->vtbl->SetObjectArrayElement(env, arr, index, str);
        env->vtbl->DeleteLocalRef(env, str);

        ++index;
    }

    return arr;
}
