#include "Compiler.h"
#include "Compiler/Tree/SourceLocation.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/Linker/Program.h"
#include "Compiler/Linker/Linker.h"
#include "Compiler/Linker/CompiledOutput.h"
#include "Compiler/Assembler/AssemblerParser.h"
#include "Compiler/Output/TRDOSWriter.h"
#include "Compiler/SourceFile.h"
#include "Compiler/SpectrumBasicCompiler.h"
#include "Compiler/Lexer.h"
#include "Compiler/Project.h"
#include "Common/IO.h"
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>

namespace
{
    struct BasicFile
    {
        std::string name;
        std::string data;
        int startLine;
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

void Compiler::buildProject(const std::filesystem::path& projectFile, const std::string& projectConfiguration)
{
    // Read project file

    if (mListener)
        mListener->compilerProgress(0, 0, "Reading project file...");

    Project project;
    project.load(projectFile);

    std::string projectName = projectFile.stem().string();

    mProjectPath = projectFile;
    mProjectPath.remove_filename();

    if (project.outputDirectory)
        mOutputPath = mProjectPath / pathFromUtf8(*project.outputDirectory);
    else
        mOutputPath = mProjectPath / Project::DefaultOutputDirectory;
    mOutputPath = mOutputPath.lexically_normal();

    // Collect list of source files

    if (mListener)
        mListener->compilerProgress(0, 0, "Scanning directories...");

    std::map<std::string, std::vector<SourceFile>> basicFiles;
    std::vector<SourceFile> sourceFiles;
    int nBasic = 0;

    SourceFile sourceFile;
    for (const auto& it : std::filesystem::recursive_directory_iterator(mProjectPath)) {
        if (it.is_directory())
            continue;

        auto ext = it.path().extension();
        if (ext == ".asm") {
            if (initSourceFile(sourceFile, FileType::Asm, it.path()))
                sourceFiles.emplace_back(sourceFile);
        } else if (ext == ".bas") {
            if (initSourceFile(sourceFile, FileType::Basic, it.path())) {
                ++nBasic;
                basicFiles[it.path().stem().string()].emplace_back(sourceFile);
            }
        }
    }

    std::sort(sourceFiles.begin(), sourceFiles.end());
    for (auto& it : basicFiles) {
        if (it.second.size() > 1)
            std::sort(it.second.begin(), it.second.end());
    }

    int n = int(sourceFiles.size());
    int total = n + 1 + nBasic + project.outputs.size();
    int count = 0;

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
        mListener->compilerProgress(count++, total, "Linking binaries...");

    Linker linker(&mHeap, &project, projectConfiguration);
    auto linkerOutput = linker.link(program);

    // Compile basic files

    std::vector<BasicFile> compiledBasicFiles;

    for (const auto& it : basicFiles) {
        SpectrumBasicCompiler compiler(&mHeap, linkerOutput);

        for (const auto& file : it.second) {
            if (mListener)
                mListener->compilerProgress(count++, total, file.fileID->name().string());
            compiler.addFile(&file);
        }

        compiler.compile();

        int startLine = -1;
        if (compiler.hasStartLine())
            startLine = compiler.startLine();

        compiledBasicFiles.emplace_back(BasicFile{ it.first, compiler.compiledData(), startLine });
    }

    // Generate separate files

    std::filesystem::path individualFilesPath = mOutputPath / "files";

    for (const auto& file : linkerOutput->files()) {
        size_t n = file->size();
        const auto* p = file->data();

        std::unique_ptr<uint8_t[]> bytes{new uint8_t[n]};
        for (size_t i = 0; i < n; i++)
            bytes[i] = p[i].value;

        writeFile(individualFilesPath / file->name(), bytes.get(), n);
    }

    for (const auto& it : compiledBasicFiles)
        writeFile(individualFilesPath / (it.name + ".B"), it.data);

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
                for (const auto& it : compiledBasicFiles)
                    writer.addBasicFile(it.name, it.data, it.startLine);
                for (const auto& file : linkerOutput->files())
                    writer.addCodeFile(file->name(), file->data(), file->size(), file->loadAddress());
                writer.writeSclFile(mOutputPath / (projectName + ".scl"));
                writer.writeTrdFile(mOutputPath / (projectName + ".trd"), projectName);
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

bool Compiler::initSourceFile(SourceFile& sourceFile, FileType fileType, const std::filesystem::path& filePath)
{
    auto currentPath = filePath.lexically_normal();
    auto rootEnd = std::mismatch(mOutputPath.begin(), mOutputPath.end(), currentPath.begin()).first;
    if (rootEnd == mOutputPath.end())
        return false;

    auto name = currentPath.lexically_relative(mProjectPath);
    sourceFile.fileID = new (&mHeap) FileID(name, filePath);
    sourceFile.fileType = fileType;
    return true;
}
