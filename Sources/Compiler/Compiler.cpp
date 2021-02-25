#include "Compiler.h"
#include "Compiler/Tree/SourceLocation.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/Linker/Program.h"
#include "Compiler/Linker/Linker.h"
#include "Compiler/Assembler/AssemblerParser.h"
#include "Compiler/SourceFile.h"
#include "Compiler/BasicCompiler.h"
#include "Compiler/Lexer.h"
#include "Compiler/Project.h"
#include "Common/IO.h"
#include <vector>
#include <algorithm>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Compiler::Compiler(ICompilerListener* listener)
    : mListener(listener)
{
}

Compiler::~Compiler()
{
}

void Compiler::buildProject(const std::filesystem::path& projectFile, const std::string& projectConfiguration)
{
    if (mListener)
        mListener->compilerProgress(0, 0, "Reading project file...");

    Project project;
    project.load(projectFile);

    if (mListener)
        mListener->compilerProgress(0, 0, "Scanning directories...");

    std::vector<SourceFile> sourceFiles;
    std::vector<SourceFile> basicFiles;

    std::filesystem::path dir = projectFile;
    dir.remove_filename();
    for (const auto& it : std::filesystem::recursive_directory_iterator(dir)) {
        if (it.is_directory())
            continue;

        FileType fileType;
        std::vector<SourceFile>* list;

        auto ext = it.path().extension();
        if (ext == ".asm") {
            fileType = FileType::Asm;
            list = &sourceFiles;
        } else if (ext == ".bas") {
            fileType = FileType::Basic;
            list = &basicFiles;
        } else
            continue;

        auto name = it.path().lexically_relative(dir);
        list->emplace_back(SourceFile{ fileType, new (&mHeap) FileID(name, it.path()) });
    }

    std::sort(sourceFiles.begin(), sourceFiles.end());

    if (mListener)
        mListener->compilerProgress(0, 0, "Compiling...");

    int n = int(sourceFiles.size());
    int nBasic = int(basicFiles.size());
    int count = 0;
    int total = n + nBasic + 3;

    auto program = new (&mHeap) Program();

    for (int i = 0; i < n; i++) {
        const auto& file = sourceFiles[i];
        if (mListener)
            mListener->compilerProgress(count++, total, file.fileID->name().string());

        switch (file.fileType) {
            case FileType::Asm: {
                Lexer lexer(&mHeap);
                lexer.scan(file.fileID, loadFile(file.fileID->path()).c_str());
                AssemblerParser parser(&mHeap, program);
                parser.parse(lexer.firstToken());
                break;
            }
        }
    }

    if (mListener)
        mListener->compilerProgress(count++, total, "Linking...");

    Linker linker(&mHeap, &project, projectConfiguration);
    auto linkerOutput = linker.link(program);

    if (mListener)
        mListener->compilerProgress(count++, total, "Reading BASIC code...");

    BasicCompiler compiler(&mHeap, linkerOutput);

    for (int i = 0; i < nBasic; i++) {
        auto& file = basicFiles[i];
        if (mListener)
            mListener->compilerProgress(count++, total, file.fileID->name().string());

        switch (file.fileType) {
            case FileType::Basic: {
                compiler.addFile(&file);
                break;
            }
        }
    }

    if (mListener)
        mListener->compilerProgress(count++, total, "Producing BASIC code...");

    compiler.compile();

    if (mListener)
        mListener->compilerProgress(count++, total, "Generating output...");

    /* FIXME */

    if (mListener)
        mListener->compilerProgress(count, total, "Done");
}
