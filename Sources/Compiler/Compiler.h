#ifndef COMPILER_COMPILER_H
#define COMPILER_COMPILER_H

#include "Common/Common.h"
#include <optional>
#include <filesystem>

class GCHeap;
class CompiledOutput;
struct SourceFile;
enum class FileType;

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
    explicit Compiler(GCHeap* heap, ICompilerListener* listener = nullptr);
    ~Compiler();

    void setJvmDllPath(std::filesystem::path path);

    CompiledOutput* linkerOutput() const { return mLinkerOutput; }
    const std::optional<std::filesystem::path>& generatedWavFile() const { return mGeneratedWavFile; }

    void setEnableWav(bool flag) { mEnableWav = flag; }

    void buildProject(const std::filesystem::path& projectFile, const std::string& projectConfiguration);

private:
    GCHeap* mHeap;
    ICompilerListener* mListener;
    CompiledOutput* mLinkerOutput;
    std::optional<std::filesystem::path> mJvmDllPath;
    std::filesystem::path mProjectPath;
    std::filesystem::path mOutputPath;
    std::optional<std::filesystem::path> mGeneratedWavFile;
    bool mEnableWav;

    bool initSourceFile(SourceFile& sourceFile, FileType fileType, const std::filesystem::path& filePath);

    DISABLE_COPY(Compiler);
};

#endif
