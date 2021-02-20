#ifndef COMPILER_PROJECT_H
#define COMPILER_PROJECT_H

#include "Common/Common.h"
#include <string>

class Project
{
public:
    static const char* FileSuffix;

    Project();
    ~Project();

    void load(const std::string& file);
    void save(const std::string& file, bool createNew = false);

    DISABLE_COPY(Project);
};

#endif
