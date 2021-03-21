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
#include "Compiler/CompilerError.h"
#include "Compiler/Lexer.h"
#include "Compiler/Project.h"
#include "Common/IO.h"
#include "Common/GC.h"
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>

namespace
{
    struct BasicFile
    {
        std::string data;
        int startLine;
    };
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Compiler::Compiler(GCHeap* heap, ICompilerListener* listener)
    : mHeap(heap)
    , mListener(listener)
    , mLinkerOutput(nullptr)
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

    auto program = new (mHeap) Program();

    // Compile source files

    auto projectVariables = new (mHeap) SymbolTable(nullptr);
    project.setVariables(program->projectVariables(), projectConfiguration);

    for (int i = 0; i < n; i++) {
        const auto& file = sourceFiles[i];
        if (mListener)
            mListener->compilerProgress(count++, total, file.fileID->name().string());

        switch (file.fileType) {
            case FileType::Asm: {
                Lexer lexer(mHeap, Lexer::Mode::Assembler);
                lexer.scan(file.fileID, loadFile(file.fileID->path()).c_str());
                AssemblerParser parser(mHeap, program);
                parser.parse(lexer.firstToken());
                break;
            }
        }
    }

    // Link program

    if (mListener)
        mListener->compilerProgress(count++, total, "Linking binaries...");

    Linker linker(mHeap, &project);
    mLinkerOutput = linker.link(program);

    // Compile basic files

    std::unordered_map<std::string, BasicFile> compiledBasicFiles;

    for (const auto& it : basicFiles) {
        SpectrumBasicCompiler compiler(mHeap, mLinkerOutput);

        for (const auto& file : it.second) {
            if (mListener)
                mListener->compilerProgress(count++, total, file.fileID->name().string());
            compiler.addFile(&file);
        }

        compiler.compile();

        int startLine = -1;
        if (compiler.hasStartLine())
            startLine = compiler.startLine();

        compiledBasicFiles[it.first] = BasicFile{ compiler.compiledData(), startLine };
    }

    // Generate separate files

    std::filesystem::path individualFilesPath = mOutputPath / "files";

    for (const auto& file : mLinkerOutput->files()) {
        size_t n = file->size();
        const auto* p = file->data();

        std::unique_ptr<uint8_t[]> bytes{new uint8_t[n]};
        for (size_t i = 0; i < n; i++)
            bytes[i] = p[i].value;

        writeFile(individualFilesPath / file->name(), bytes.get(), n);
    }

    for (const auto& it : compiledBasicFiles)
        writeFile(individualFilesPath / (it.first + ".B"), it.second.data);

    // Generate outputs configured in the project

    for (const auto& output : project.outputs) {
        std::unique_ptr<IOutputWriter> writer;

        if (!output->isEnabled(program->projectVariables()))
            continue;

        switch (output->type) {
            case Project::Output::ZXSpectrumTAP:
                if (mListener)
                    mListener->compilerProgress(count++, total, "Generating TAP...");
                // FIXME
                throw CompilerError(nullptr, "Not implemented");
                continue;

            case Project::Output::ZXSpectrumTRD: {
                if (mListener)
                    mListener->compilerProgress(count++, total, "Generating TRD and SCL...");

                auto trdosWriter = std::make_unique<TRDOSWriter>();
                trdosWriter->setWriteSclFile(mOutputPath / (projectName + ".scl"));
                trdosWriter->setWriteTrdFile(mOutputPath / (projectName + ".trd"), projectName);

                writer = std::move(trdosWriter);
                break;
            }

            default:
                assert(false);
                ++count;
                continue;
        }

        for (const auto& file : output->files) {
            if (file.ref) {
                auto data = mLinkerOutput->getFile(*file.ref);
                if (!data) {
                    std::stringstream ss;
                    ss << "File \"" << *file.ref << "\" was not generated by the compiler.";
                    throw CompilerError(file.location, ss.str());
                }
                writer->addCodeFile(*file.ref, data->data(), data->size(), data->loadAddress());
            } else if (file.basic) {
                auto it = compiledBasicFiles.find(*file.basic);
                if (it == compiledBasicFiles.end()) {
                    std::stringstream ss;
                    ss << "Basic file \"" << *file.basic << "\" was not found.";
                    throw CompilerError(file.location, ss.str());
                }
                writer->addBasicFile(it->first, it->second.data, it->second.startLine);
            }
            else
                throw CompilerError(file.location, "internal compiler error: unsupported output file.");
        }

        writer->writeOutput();
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
    sourceFile.fileID = new (mHeap) FileID(name, filePath);
    sourceFile.fileType = fileType;
    return true;
}
