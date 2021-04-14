#ifndef COMPILER_LINKER_LINKER_H
#define COMPILER_LINKER_LINKER_H

#include "Common/Common.h"
#include "Compiler/Linker/ISectionResolver.h"

class GCHeap;
class Project;
class Program;
class CompiledOutput;

class Linker : public ISectionResolver
{
public:
    Linker(GCHeap* heap, const Project* project);
    ~Linker();

    CompiledOutput* link(Program* program);

    bool isValidSectionName(SourceLocation* location, const std::string& name) const override;
    bool tryResolveSectionAddress(SourceLocation* location, const std::string& name, uint64_t& value) const override;
    bool tryResolveSectionBase(SourceLocation* location, const std::string& name, uint64_t& value) const override;
    bool tryResolveSectionSize(SourceLocation* location, const std::string& name, uint64_t& value) const override;

private:
    class LinkerFile;

    GCHeap* mHeap;
    const Project* mProject;
    Program* mProgram;
    std::unordered_set<std::string> mFileNames;
    std::unordered_set<std::string> mUsedSections;
    std::vector<LinkerFile*> mFiles;

    DISABLE_COPY(Linker);
};

#endif
