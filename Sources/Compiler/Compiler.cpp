#include "Compiler.h"
#include "Compiler/Tree/SourceLocation.h"
#include "Compiler/Project.h"
#include <vector>
#include <sstream>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace
{
    enum class FileType
    {
        Asm,
    };

    struct SourceFile
    {
        FileType fileType;
        FileID* fileID;
    };
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Compiler::Compiler(ICompilerListener* listener)
    : mListener(listener)
{
}

Compiler::~Compiler()
{
}

void Compiler::buildProject(const std::filesystem::path& projectFile)
{
    GCHeap heap;

    if (mListener)
        mListener->compilerProgress(0, 0, "Reading project file...");

    Project project;
    project.load(projectFile);

    if (mListener)
        mListener->compilerProgress(0, 0, "Scanning directories...");

    std::vector<SourceFile> files;

    std::filesystem::path dir = projectFile;
    dir.remove_filename();
    for (const auto& it : std::filesystem::recursive_directory_iterator(dir)) {
        if (it.is_directory())
            continue;

        FileType fileType;
        auto ext = it.path().extension();
        if (ext == ".asm")
            fileType = FileType::Asm;
        else
            continue;

        auto path = it.path().lexically_relative(dir);
        files.emplace_back(SourceFile{ fileType, new (&heap) FileID(path) });
    }

    int n = int(files.size());
    int count = 0;
    int total = n + 1;

    for (int i = 0; i < n; i++) {
        const auto& file = files[i];
        mListener->compilerProgress(count++, total, file.fileID->path().string());
        Sleep(1000);
    }

    mListener->compilerProgress(count++, total, "Linking...");
    Sleep(1000);

    mListener->compilerProgress(count, total, "Done");
}
