#ifndef COMPILER_LINKER_LINKER_H
#define COMPILER_LINKER_LINKER_H

#include "Common/Common.h"
#include <string>

class GCHeap;
class Project;
class Program;
class CompiledOutput;

class Linker
{
public:
    Linker(GCHeap* heap, const Project* project, std::string projectConfiguration);
    ~Linker();

    CompiledOutput* link(Program* program);

private:
    GCHeap* mHeap;
    const Project* mProject;
    Program* mProgram;
    std::string mProjectConfiguration;

    DISABLE_COPY(Linker);
};

#endif
