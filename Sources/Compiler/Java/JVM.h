#ifndef JVM_H
#define JVM_H

#include "Common/Common.h"
#include <filesystem>

class JVM
{
public:
    static bool isLoaded();
    static const std::filesystem::path& loadedDllPath();

    static std::filesystem::path findJvmDll(const std::filesystem::path& jdkPath);
    static std::filesystem::path findJavaC(const std::filesystem::path& jdkPath);

    static void load(std::filesystem::path dllPath);
    static void destroy();

private:
    DISABLE_COPY(JVM);
};

#endif
