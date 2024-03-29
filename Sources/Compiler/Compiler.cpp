#include "Compiler.h"
#include "Compiler/Tree/Value.h"
#include "Compiler/Tree/SourceLocation.h"
#include "Compiler/Tree/SourceLocationFactory.h"
#include "Compiler/Tree/Symbol.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/Tree/Expr.h"
#include "Compiler/Java/JNIThrowableRef.h"
#include "Compiler/Java/JStringList.h"
#include "Compiler/Java/JVMThreadContext.h"
#include "Compiler/Java/JavaClasses.h"
#include "Compiler/Java/JVM.h"
#include "Compiler/Linker/Program.h"
#include "Compiler/Linker/Linker.h"
#include "Compiler/Linker/CompiledOutput.h"
#include "Compiler/Assembler/AssemblerParser.h"
#include "Compiler/Output/TRDOSWriter.h"
#include "Compiler/Output/SpectrumSnapshotWriter.h"
#include "Compiler/Output/SpectrumTapeWriter.h"
#include "Compiler/Output/IOutputWriterProxy.h"
#include "Compiler/SourceFile.h"
#include "Compiler/SpectrumBasicCompiler.h"
#include "Compiler/CompilerError.h"
#include "Compiler/Lexer.h"
#include "Compiler/Project.h"
#include "Common/IO.h"
#include "Common/GC.h"
#include "Common/Strings.h"

namespace
{
    struct BasicFile
    {
        std::string data;
        int startLine;
    };
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Compiler::Compiler(GCHeap* heap, const std::filesystem::path& resourcesPath, ICompilerListener* listener)
    : mHeap(heap)
    , mListener(listener)
    , mLinkerOutput(nullptr)
    , mOutputWriterProxy(nullptr)
    , mJVMThreadContext(new JVMThreadContext(mHeap))
    , mResourcesPath(resourcesPath / "data")
    , mEnableWav(false)
    , mShouldDetachJVM(false)
{
    mJVMThreadContext->setListener(mListener);
}

Compiler::~Compiler()
{
    if (mShouldDetachJVM) {
        try {
            JVM::detachCurrentThread();
        } catch (const CompilerError& e) {
            mListener->printMessage(e.message());
        } catch (const std::exception& e) {
            mListener->printMessage(e.what());
        }
    }

    mJVMThreadContext.reset();
}

void Compiler::setJdkPath(std::filesystem::path path)
{
    mJdkPath = std::move(path);
}

void Compiler::buildProject(const std::filesystem::path& projectFile, const std::string& projectConfiguration)
{
    // Read project file

    if (mListener)
        mListener->compilerProgress(0, 0, "Reading project file...");

    Project project;
    SourceLocationFactory locationFactory(mHeap);
    project.load(projectFile, &locationFactory);

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
    std::vector<SourceFile> gameJavaFiles;
    std::vector<SourceFile> buildJavaFiles;
    std::vector<SourceFile> asmSourceFiles;
    int nBasic = 0;

    SourceFile sourceFile;
    for (const auto& it : std::filesystem::recursive_directory_iterator(mProjectPath)) {
        if (it.is_directory())
            continue;

        FileType fileType = SourceFile::determineFileType(it.path());
        switch (fileType) {
            case FileType::Unknown:
                break;

            case FileType::Asm:
                if (initSourceFile(sourceFile, FileType::Asm, it.path()))
                    asmSourceFiles.emplace_back(sourceFile);
                break;

            case FileType::Java:
                if (initSourceFile(sourceFile, FileType::Java, it.path())) {
                    std::string prefix = sourceFile.fileID->name().string();
                    if (!startsWith(prefix, "build/") && !startsWith(prefix, "build\\"))
                        gameJavaFiles.emplace_back(sourceFile);
                    else
                        buildJavaFiles.emplace_back(sourceFile);
                }
                break;

            case FileType::Basic:
                if (initSourceFile(sourceFile, FileType::Basic, it.path())) {
                    ++nBasic;
                    basicFiles[it.path().stem().string()].emplace_back(sourceFile);
                }
                break;
        }
    }

    std::sort(gameJavaFiles.begin(), gameJavaFiles.end());
    std::sort(buildJavaFiles.begin(), buildJavaFiles.end());

    int nAsm = int(asmSourceFiles.size());
    int count = 0;
    int total = 1
              + nAsm
              + nBasic
              + (buildJavaFiles.empty() ? 0 : 1)
              + (buildJavaFiles.empty() && gameJavaFiles.empty() ? 0 : 5)
              + project.outputs.size();

    auto program = new (mHeap) Program();

    auto projectVariables = new (mHeap) SymbolTable(nullptr);
    project.setVariables(program->projectVariables(), projectConfiguration);

    // Compile java files

    if (!buildJavaFiles.empty() || !gameJavaFiles.empty()) {
        if (mListener)
            mListener->compilerProgress(count++, total, "Initializing Java Virtual Machine...");

        if (JVM::isLoaded()) {
            if (!JVM::isAttached()) {
                mShouldDetachJVM = true;
                JVM::attachCurrentThread();
            }
        } else {
            if (!mJdkPath.has_value())
                throw CompilerError(nullptr, "JDK path was not specified.");

            mShouldDetachJVM = true;
            JVM::load(*mJdkPath, mResourcesPath / "RetroBuild.jar");
        }

        int version = JVM::majorVersion();
        const char* targetVersion = "1.5";
        if (version >= 8)
            targetVersion = "1.8";
        else if (version >= 7)
            targetVersion = "1.7";
        else if (version >= 6)
            targetVersion = "1.6";

        // Generate constants for Java scripts

        if (mListener)
            mListener->compilerProgress(count++, total, "Generating Java code...");

        std::vector<const ConstantSymbol*> symbols;
        symbols.reserve(program->projectVariables()->symbols().size());
        for (const auto& it : program->projectVariables()->symbols()) {
            assert(it.second->type() == Symbol::Constant);
            if (it.second->type() == Symbol::Constant)
                symbols.emplace_back(static_cast<ConstantSymbol*>(it.second));
        }

        std::sort(symbols.begin(), symbols.end(), [](const ConstantSymbol* a, const ConstantSymbol* b) -> bool {
                return strcmp(a->name(), b->name()) < 0;
            });

        std::stringstream ss;
        ss << "public final class BuildSettings\n";
        ss << "{\n";
        for (const ConstantSymbol* symbol : symbols) {
            std::unique_ptr<CompilerError> error;
            if (!symbol->value()->canEvaluateValue(nullptr, nullptr, error))
                ss << "    // Skipped " << symbol->name() << ": " << error->message() << "\n";
            else {
                auto value = symbol->value()->evaluateValue(nullptr, nullptr);
                const char* type = (value.number > 0x7fffffff ? "long" : "int");
                ss << "    public static final " << type << ' ' << symbol->name() << " = " << value.number << ";\n";
            }
        }
        ss << "};\n";
        std::string buildSettingsJava = ss.str();

        auto path = mOutputPath / "java_generated" / "build" / "BuildSettings.java";
        writeFile(path, "package build;\n" + buildSettingsJava, SkipIfSameContent);
        sourceFile.fileID = new (mHeap) FileID("build/BuildSettings.java", path);
        sourceFile.fileType = FileType::Java;
        buildJavaFiles.emplace_back(sourceFile);

        path = mOutputPath / "java_generated" / "game" / "BuildSettings.java";
        writeFile(path, "package game;\n" + buildSettingsJava, SkipIfSameContent);
        sourceFile.fileID = new (mHeap) FileID("game/BuildSettings.java", path);
        sourceFile.fileType = FileType::Java;
        gameJavaFiles.emplace_back(sourceFile);

        // Game code

        if (mListener)
            mListener->compilerProgress(count++, total, "Compiling Java game code...");

        if (!gameJavaFiles.empty()) {
            JStringList list;
            list.reserve(gameJavaFiles.size() + 12);
            list.add("-Xlint:all");
            list.add("-g");
            list.add("-encoding");
            list.add("UTF-8");
            list.add("-source");
            list.add(targetVersion);
            list.add("-target");
            list.add(targetVersion);
            list.add("-bootclasspath");
            list.add(mResourcesPath / "RetroEngine.jar");
            list.add("-sourcepath");
            list.add(mProjectPath);
            list.add("-d");
            list.add(mOutputPath / "java");
            for (const auto& file : gameJavaFiles)
                list.add(file.fileID->path());

            if (!JVM::compile(list)) {
                JNIThrowableRef::rethrowCurrentException();
                throw CompilerError(nullptr, "Java compilation failed.");
            }
        }

        // Build and run tools

        if (mListener)
            mListener->compilerProgress(count++, total, "Compiling and running Java build scripts...");

        if (!buildJavaFiles.empty()) {
            JStringList list;
            list.reserve(buildJavaFiles.size() + 8);
            list.add("-Xlint:all");
            list.add("-g");
            list.add("-encoding");
            list.add("UTF-8");
            list.add("-classpath");
            list.add(mResourcesPath / "RetroBuild.jar");
            list.add("-sourcepath");
            list.add(mProjectPath);
            list.add("-d");
            list.add(mOutputPath / "java");
            for (const auto& file : buildJavaFiles)
                list.add(file.fileID->path());

            if (!JVM::compile(list)) {
                JNIThrowableRef::rethrowCurrentException();
                throw CompilerError(nullptr, "Java compilation failed.");
            }

            if (mListener)
                mListener->compilerProgress(count++, total, "Running Java tools...");

            JStringList classpath;
            classpath.add(mProjectPath / "!*.class");
            classpath.add(mResourcesPath / "RetroBuild.jar");
            classpath.add(mOutputPath / "java" / "*.class");
            classpath.add(mOutputPath / "generated" / "=>");

            list.clear();
            for (const auto& file : buildJavaFiles) {
                list.add(file.fileID->name());
                list.add(file.fileID->path());
            }

            if (!JVM::runClass(JavaClasses::drunkfly_internal_BuilderLauncher.name().c_str(), list, true, &classpath)) {
                JNIThrowableRef::rethrowCurrentException();
                throw CompilerError(nullptr, "Error running build tool with Java.");
            }
        }
    }

    // Add generated source files

    for (const auto& file : mJVMThreadContext->generatedFiles()) {
        switch (file.fileType) {
            case FileType::Java:
            case FileType::Unknown:
                break;

            case FileType::Asm:
                ++total;
                ++nAsm;
                asmSourceFiles.emplace_back(file);
                break;

            case FileType::Basic:
                ++total;
                ++nBasic;
                basicFiles[file.fileID->path().stem().string()].emplace_back(file);
                break;
        }
    }

    std::sort(asmSourceFiles.begin(), asmSourceFiles.end());

    for (auto& it : basicFiles) {
        if (it.second.size() > 1)
            std::sort(it.second.begin(), it.second.end());
    }

    // Compile source files

    for (int i = 0; i < nAsm; i++) {
        const auto& file = asmSourceFiles[i];
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

            default:
                throw CompilerError(
                    new (mHeap) SourceLocation(file.fileID, 0), "Internal compiler error: invalid file type.");
        }
    }

    // Link program

    if (mListener)
        mListener->compilerProgress(count++, total, "Linking...");

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

    std::unordered_set<std::string> outputs;
    auto makePath = [this, &outputs](std::string name) -> std::filesystem::path {
            std::filesystem::path path = mOutputPath / name;
            if (!outputs.emplace(std::move(name)).second) {
                std::stringstream ss;
                ss << "Duplicate output file \"" << path << "\".";
                throw CompilerError(nullptr, ss.str());
            }
            return path;
        };

    for (const auto& output : project.outputs) {
        std::unique_ptr<IOutputWriter> outputWriter;

        if (!output->isEnabled(program->projectVariables()))
            continue;

        switch (output->type) {
            case Project::Output::ZXSpectrumTAP: {
                if (mListener)
                    mListener->compilerProgress(count++, total, "Generating TAP...");

                auto tapeWriter = std::make_unique<SpectrumTapeWriter>();
                tapeWriter->setWriteTapFile(makePath(projectName + ".tap"));
                if (mEnableWav) {
                    mGeneratedWavFile = makePath(projectName + ".wav");
                    tapeWriter->setWriteWavFile(*mGeneratedWavFile);
                }

                outputWriter = std::move(tapeWriter);
                break;
            }

            case Project::Output::ZXSpectrumTRD: {
                if (mListener)
                    mListener->compilerProgress(count++, total, "Generating TRD and SCL...");

                auto trdosWriter = std::make_unique<TRDOSWriter>();
                trdosWriter->setWriteSclFile(makePath(projectName + ".scl"));
                trdosWriter->setWriteTrdFile(makePath(projectName + ".trd"), projectName);

                outputWriter = std::move(trdosWriter);
                break;
            }

            case Project::Output::ZXSpectrumZ80: {
                if (mListener)
                    mListener->compilerProgress(count++, total, "Generating Z80...");

                auto z80Writer = std::make_unique<SpectrumSnapshotWriter>();
                output->z80->initWriter(program, &linker, z80Writer.get());
                z80Writer->setWriteZ80File(output->location, makePath(projectName + ".z80"));

                outputWriter = std::move(z80Writer);
                break;
            }

            case Project::Output::PC: {
                if (mListener)
                    mListener->compilerProgress(count++, total, "Generating executables...");

                auto z80Writer = std::make_unique<SpectrumSnapshotWriter>();
                output->z80->initWriter(program, &linker, z80Writer.get());
                z80Writer->addWriteExeFile(output->location,
                    mResourcesPath / "Win32Runtime.bin", makePath(projectName + ".exe"));

                outputWriter = std::move(z80Writer);
                break;
            }

            default:
                assert(false);
                ++count;
                continue;
        }

        IOutputWriter* writer = outputWriter.get();
        if (mOutputWriterProxy) {
            mOutputWriterProxy->setOutput(output->type, outputWriter.get());
            writer = mOutputWriterProxy;
        }

        for (const auto& file : output->files) {
            if (file.ref) {
                auto data = mLinkerOutput->getFile(*file.ref);
                if (!data) {
                    std::stringstream ss;
                    ss << "File \"" << *file.ref << "\" was not generated by the compiler.";
                    throw CompilerError(file.location, ss.str());
                }
                std::string name = (file.name.has_value() ? *file.name : *file.ref);
                writer->addCodeFile(file.location,
                    std::move(name), *file.ref, data->data(), data->size(), data->loadAddress());
            } else if (file.refBasic) {
                auto it = compiledBasicFiles.find(*file.refBasic);
                if (it == compiledBasicFiles.end()) {
                    std::stringstream ss;
                    ss << "Basic file \"" << *file.refBasic << "\" was not found.";
                    throw CompilerError(file.location, ss.str());
                }
                std::string name = (file.name.has_value() ? *file.name : *file.refBasic);
                writer->addBasicFile(file.location, std::move(name), it->second.data, it->second.startLine);
            } else
                throw CompilerError(file.location, "Internal compiler error: unsupported output file.");
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
