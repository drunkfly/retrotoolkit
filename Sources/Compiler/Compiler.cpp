#include "Compiler.h"
#include "Compiler/Tree/SourceLocation.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/Linker/Program.h"
#include "Compiler/Assembler/AssemblerParser.h"
#include "Compiler/Lexer.h"
#include "Compiler/Project.h"
#include "Common/IO.h"
#include <vector>
#include <algorithm>
#include <sstream>

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

        bool operator<(const SourceFile& other) const
        {
            if (fileType < other.fileType)
                return true;
            else if (fileType > other.fileType)
                return false;

            return (fileID->name() < other.fileID->name());
        }
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

        auto name = it.path().lexically_relative(dir);
        files.emplace_back(SourceFile{ fileType, new (&mHeap) FileID(name, it.path()) });
    }

    std::sort(files.begin(), files.end());

    if (mListener)
        mListener->compilerProgress(0, 0, "Compiling...");

    int n = int(files.size());
    int count = 0;
    int total = n + 1;

    for (int i = 0; i < n; i++) {
        const auto& file = files[i];
        if (mListener)
            mListener->compilerProgress(count++, total, file.fileID->name().string());

        switch (file.fileType) {
            case FileType::Asm: {
                Lexer lexer(&mHeap);
                lexer.scan(file.fileID, loadFile(file.fileID->path()).c_str());
                auto program = new (&mHeap) Program();
                AssemblerParser parser(&mHeap, program);
                parser.parse(lexer.firstToken());
                break;
            }
        }
    }

    if (mListener)
        mListener->compilerProgress(count++, total, "Linking...");

    /* FIXME */

    if (mListener)
        mListener->compilerProgress(count, total, "Done");
}
