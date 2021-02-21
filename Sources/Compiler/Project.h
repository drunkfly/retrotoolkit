#ifndef COMPILER_PROJECT_H
#define COMPILER_PROJECT_H

#include "Common/Common.h"
#include <filesystem>

class Project
{
public:
    static const char* FileSuffix;

    Project();
    ~Project();

    void load(const std::filesystem::path& file);
    void save(const std::filesystem::path& file, bool createNew = false);

    DISABLE_COPY(Project);
};

#endif
