#include "Compiler.h"
#include "Compiler/Tree/Value.h"
#include "Compiler/Tree/SourceLocation.h"
#include "Compiler/Tree/SourceLocationFactory.h"
#include "Compiler/Tree/SymbolTable.h"
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

Compiler::Compiler(GCHeap* heap, std::filesystem::path resourcesPath, ICompilerListener* listener)
    : mHeap(heap)
    , mListener(listener)
    , mLinkerOutput(nullptr)
    , mJVMThreadContext(new JVMThreadContext(mHeap))
    , mResourcesPath(std::move(resourcesPath))
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

    delete mJVMThreadContext;
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
              + (buildJavaFiles.empty() && gameJavaFiles.empty() ? 0 : 3)
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

        // Game code

        if (mListener)
            mListener->compilerProgress(count++, total, "Compiling Java game code...");

        if (!gameJavaFiles.empty()) {
            JStringList list;
            list.reserve(gameJavaFiles.size() + 12);
            list.add("-Xlint:all");
            list.add("-g");
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

    for (const auto& output : project.outputs) {
        std::unique_ptr<IOutputWriter> writer;

        if (!output->isEnabled(program->projectVariables()))
            continue;

        switch (output->type) {
            case Project::Output::ZXSpectrumTAP: {
                if (mListener)
                    mListener->compilerProgress(count++, total, "Generating TAP...");

                auto tapeWriter = std::make_unique<SpectrumTapeWriter>();
                tapeWriter->setWriteTapFile(mOutputPath / (projectName + ".tap"));
                if (mEnableWav) {
                    mGeneratedWavFile = mOutputPath / (projectName + ".wav");
                    tapeWriter->setWriteWavFile(*mGeneratedWavFile);
                }

                writer = std::move(tapeWriter);
                break;
            }

            case Project::Output::ZXSpectrumTRD: {
                if (mListener)
                    mListener->compilerProgress(count++, total, "Generating TRD and SCL...");

                auto trdosWriter = std::make_unique<TRDOSWriter>();
                trdosWriter->setWriteSclFile(mOutputPath / (projectName + ".scl"));
                trdosWriter->setWriteTrdFile(mOutputPath / (projectName + ".trd"), projectName);

                writer = std::move(trdosWriter);
                break;
            }

            case Project::Output::ZXSpectrumZ80: {
                if (mListener)
                    mListener->compilerProgress(count++, total, "Generating Z80...");

                auto z80Writer = std::make_unique<SpectrumSnapshotWriter>();
                z80Writer->setWriteZ80File(output->location, mOutputPath / (projectName + ".z80"));

                if (output->z80->format.value.has_value()) {
                    if (*output->z80->format.value == "auto")
                        z80Writer->setFormat(Z80Format::Auto);
                    else {
                        auto value = output->z80->format.evaluateValue(program, &linker).number;
                        if (value == 1)
                            z80Writer->setFormat(Z80Format::Version1);
                        else if (value == 2)
                            z80Writer->setFormat(Z80Format::Version2);
                        else if (value == 3)
                            z80Writer->setFormat(Z80Format::Version3);
                        else
                            throw CompilerError(output->z80->format.location, "invalid Z80 format.");
                    }
                }

                if (output->z80->machine.value.has_value()) {
                    if (*output->z80->machine.value == "auto")
                        z80Writer->setMachine(Z80Machine::Auto);
                    else if (*output->z80->machine.value == "16k")
                        z80Writer->setMachine(Z80Machine::Spectrum16k);
                    else if (*output->z80->machine.value == "48k")
                        z80Writer->setMachine(Z80Machine::Spectrum48k);
                    else if (*output->z80->machine.value == "128k")
                        z80Writer->setMachine(Z80Machine::Spectrum128k);
                    else if (*output->z80->machine.value == "+3")
                        z80Writer->setMachine(Z80Machine::SpectrumPlus3);
                    else if (*output->z80->machine.value == "pentagon")
                        z80Writer->setMachine(Z80Machine::Pentagon);
                    else if (*output->z80->machine.value == "scorpion")
                        z80Writer->setMachine(Z80Machine::Scorpion);
                    else if (*output->z80->machine.value == "didaktik")
                        z80Writer->setMachine(Z80Machine::DidaktikKompakt);
                    else if (*output->z80->machine.value == "+2")
                        z80Writer->setMachine(Z80Machine::SpectrumPlus2);
                    else if (*output->z80->machine.value == "+2A")
                        z80Writer->setMachine(Z80Machine::SpectrumPlus2A);
                    else if (*output->z80->machine.value == "tc2048")
                        z80Writer->setMachine(Z80Machine::TC2048);
                    else if (*output->z80->machine.value == "tc2068")
                        z80Writer->setMachine(Z80Machine::TC2068);
                    else if (*output->z80->machine.value == "ts2068")
                        z80Writer->setMachine(Z80Machine::TS2068);
                    else
                        throw CompilerError(output->z80->machine.location, "invalid Z80 machine.");
                }

                if (output->z80->a.value.has_value())
                    z80Writer->setA(output->z80->a.evaluateByte(program, &linker));
                if (output->z80->f.value.has_value())
                    z80Writer->setF(output->z80->f.evaluateByte(program, &linker));
                if (output->z80->bc.value.has_value())
                    z80Writer->setBC(output->z80->bc.evaluateWord(program, &linker));
                if (output->z80->hl.value.has_value())
                    z80Writer->setHL(output->z80->hl.evaluateWord(program, &linker));
                if (output->z80->de.value.has_value())
                    z80Writer->setDE(output->z80->de.evaluateWord(program, &linker));
                if (output->z80->shadowA.value.has_value())
                    z80Writer->setShadowA(output->z80->shadowA.evaluateByte(program, &linker));
                if (output->z80->shadowF.value.has_value())
                    z80Writer->setShadowF(output->z80->shadowF.evaluateByte(program, &linker));
                if (output->z80->shadowBC.value.has_value())
                    z80Writer->setShadowBC(output->z80->shadowBC.evaluateWord(program, &linker));
                if (output->z80->shadowHL.value.has_value())
                    z80Writer->setShadowHL(output->z80->shadowHL.evaluateWord(program, &linker));
                if (output->z80->shadowDE.value.has_value())
                    z80Writer->setShadowDE(output->z80->shadowDE.evaluateWord(program, &linker));
                if (output->z80->pc.value.has_value())
                    z80Writer->setPC(output->z80->pc.evaluateWord(program, &linker));
                if (output->z80->sp.value.has_value())
                    z80Writer->setSP(output->z80->sp.evaluateWord(program, &linker));
                if (output->z80->iy.value.has_value())
                    z80Writer->setIY(output->z80->iy.evaluateWord(program, &linker));
                if (output->z80->ix.value.has_value())
                    z80Writer->setIX(output->z80->ix.evaluateWord(program, &linker));
                if (output->z80->i.value.has_value())
                    z80Writer->setI(output->z80->i.evaluateByte(program, &linker));
                if (output->z80->r.value.has_value())
                    z80Writer->setR(output->z80->r.evaluateByte(program, &linker));

                if (output->z80->port1FFD.value.has_value())
                    z80Writer->setPort1FFD(output->z80->port1FFD.evaluateWord(program, &linker));
                if (output->z80->port7FFD.value.has_value())
                    z80Writer->setPort7FFD(output->z80->port7FFD.evaluateWord(program, &linker));
                if (output->z80->portFFFD.value.has_value())
                    z80Writer->setPortFFFD(output->z80->portFFFD.evaluateWord(program, &linker));

                if (output->z80->borderColor.value.has_value()) {
                    auto color = output->z80->borderColor.evaluateValue(program, &linker).number;
                    if (color < 0 || color > 7)
                        throw CompilerError(output->z80->borderColor.location, "invalid border color.");
                    z80Writer->setBorderColor(uint8_t(color));
                }

                if (output->z80->interruptMode.value.has_value()) {
                    auto color = output->z80->interruptMode.evaluateValue(program, &linker).number;
                    if (color < 0 || color > 2)
                        throw CompilerError(output->z80->interruptMode.location, "invalid interrupt mode.");
                    z80Writer->setInterruptMode(uint8_t(color));
                }

                if (output->z80->interruptsEnabled.value.has_value())
                    z80Writer->setInterruptsEnabled(output->z80->interruptsEnabled.evaluateBool(program, &linker));

                writer = std::move(z80Writer);
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
                std::string name = (file.name.has_value() ? *file.name : *file.ref);
                writer->addCodeFile(file.location,
                    std::move(name), *file.ref, data->data(), data->size(), data->loadAddress());
            } else if (file.basic) {
                auto it = compiledBasicFiles.find(*file.basic);
                if (it == compiledBasicFiles.end()) {
                    std::stringstream ss;
                    ss << "Basic file \"" << *file.basic << "\" was not found.";
                    throw CompilerError(file.location, ss.str());
                }
                std::string name = (file.name.has_value() ? *file.name : *file.basic);
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
