#ifndef COMPILER_COMPILER_H
#define COMPILER_COMPILER_H

#include "Common/Common.h"
#include "Common/GC.h"
#include <filesystem>

class ICompilerListener
{
public:
    virtual ~ICompilerListener() = default;
    virtual void checkCancelation() const = 0;
    virtual void compilerProgress(int current, int total, const std::string& message) = 0;
};

class Compiler
{
public:
    explicit Compiler(ICompilerListener* listener = nullptr);
    ~Compiler();

    void buildProject(const std::filesystem::path& projectFile);

private:
    ICompilerListener* mListener;
    GCHeap mHeap;

    DISABLE_COPY(Compiler);
};

#endif
