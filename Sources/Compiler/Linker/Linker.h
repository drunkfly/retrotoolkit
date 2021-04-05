#ifndef COMPILER_LINKER_LINKER_H
#define COMPILER_LINKER_LINKER_H

#include "Common/Common.h"
#include "Compiler/Linker/ISectionResolver.h"

class GCHeap;
class Project;
class Program;
class CompiledOutput;

class Linker
{
public:
    Linker(GCHeap* heap, const Project* project);
    ~Linker();

    CompiledOutput* link(Program* program);

private:
    GCHeap* mHeap;
    const Project* mProject;
    Program* mProgram;

    DISABLE_COPY(Linker);
};

#endif
