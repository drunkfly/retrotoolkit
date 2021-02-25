#include "Compiler.h"
#include "Compiler/Tree/SourceLocation.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/Linker/Program.h"
#include "Compiler/Linker/Linker.h"
#include "Compiler/Linker/CompiledOutput.h"
#include "Compiler/Assembler/AssemblerParser.h"
#include "Compiler/Output/TRDOSWriter.h"
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
    // Read project file

    if (mListener)
        mListener->compilerProgress(0, 0, "Reading project file...");

    Project project;
    project.load(projectFile);

    std::string projectName = projectFile.stem().string();

    std::filesystem::path projectPath = projectFile;
    projectPath.remove_filename();

    std::filesystem::path outputPath;
    if (project.outputDirectory)
        outputPath = projectPath / pathFromUtf8(*project.outputDirectory);
    else
        outputPath = projectPath / Project::DefaultOutputDirectory;

    // Collect list of source files

    if (mListener)
        mListener->compilerProgress(0, 0, "Scanning directories...");

    std::vector<SourceFile> sourceFiles;
    std::vector<SourceFile> basicFiles;

    for (const auto& it : std::filesystem::recursive_directory_iterator(projectPath)) {
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

        auto name = it.path().lexically_relative(projectPath);
        list->emplace_back(SourceFile{ fileType, new (&mHeap) FileID(name, it.path()) });
    }

    std::sort(sourceFiles.begin(), sourceFiles.end());

    if (mListener)
        mListener->compilerProgress(0, 0, "Compiling...");

    int n = int(sourceFiles.size());
    int nBasic = int(basicFiles.size());
    int count = 0;
    int total = n + nBasic + project.outputs.size() + 2;

    auto program = new (&mHeap) Program();

    // Compile source files

    for (int i = 0; i < n; i++) {
        const auto& file = sourceFiles[i];
        if (mListener)
            mListener->compilerProgress(count++, total, file.fileID->name().string());

        switch (file.fileType) {
            case FileType::Asm: {
                Lexer lexer(&mHeap, Lexer::Mode::Assembler);
                lexer.scan(file.fileID, loadFile(file.fileID->path()).c_str());
                AssemblerParser parser(&mHeap, program);
                parser.parse(lexer.firstToken());
                break;
            }
        }
    }

    // Link program

    if (mListener)
        mListener->compilerProgress(count++, total, "Linking...");

    Linker linker(&mHeap, &project, projectConfiguration);
    auto linkerOutput = linker.link(program);

    // Compile basic files

    BasicCompiler basicCompiler(&mHeap, linkerOutput);

    for (int i = 0; i < nBasic; i++) {
        auto& file = basicFiles[i];
        if (mListener)
            mListener->compilerProgress(count++, total, file.fileID->name().string());

        switch (file.fileType) {
            case FileType::Basic: {
                basicCompiler.addFile(&file);
                break;
            }
        }
    }

    // Produce code for files

    if (mListener)
        mListener->compilerProgress(count++, total, "Processing BASIC code...");

    basicCompiler.compile();

    // Generate standard outputs

    std::filesystem::path individualFilesPath = outputPath / "files";

    if (nBasic > 0)
        writeFile(individualFilesPath / "boot.bas", basicCompiler.compiledData());

    for (const auto& file : linkerOutput->files()) {
        size_t n = file->size();
        const auto* p = file->data();

        std::unique_ptr<uint8_t[]> bytes{new uint8_t[n]};
        for (size_t i = 0; i < n; i++)
            bytes[i] = p[i].value;

        writeFile(individualFilesPath / file->name(), bytes.get(), n);
    }

    // Generate outputs configured in the project

    for (const auto& output : project.outputs) {
        switch (output->type) {
            case Project::Output::ZXSpectrumTAP:
                // FIXME
                break;

            case Project::Output::ZXSpectrumTRD: {
                if (mListener)
                    mListener->compilerProgress(count++, total, "Generating TRD and SCL...");
                TRDOSWriter writer;
                if (nBasic > 0) {
                    std::string name = (output->basicFileName ? *output->basicFileName : "BOOT");
                    int line = output->basicStartLine ? *output->basicStartLine : -1;
                    writer.addBasicFile(std::move(name), basicCompiler.compiledData(), line);
                }
                for (const auto& file : linkerOutput->files())
                    writer.addCodeFile(file->name(), file->data(), file->size(), file->loadAddress());
                writer.writeSclFile(outputPath / (projectName + ".scl"));
                writer.writeTrdFile(outputPath / (projectName + ".trd"), projectName);
                break;
            }

            default:
                assert(false);
                ++count;
                break;
        }
    }

    if (mListener)
        mListener->compilerProgress(count, total, "Done");
}
