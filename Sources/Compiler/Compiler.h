#ifndef COMPILER_COMPILER_H
#define COMPILER_COMPILER_H

#include "Common/Common.h"

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
    virtual void printMessage(std::string text) = 0;
};

class Compiler
{
public:
    Compiler(GCHeap* heap, std::filesystem::path resourcesPath, ICompilerListener* listener = nullptr);
    ~Compiler();

    void setJdkPath(std::filesystem::path path);

    CompiledOutput* linkerOutput() const { return mLinkerOutput; }
    const std::optional<std::filesystem::path>& generatedWavFile() const { return mGeneratedWavFile; }

    void setEnableWav(bool flag) { mEnableWav = flag; }

    void buildProject(const std::filesystem::path& projectFile, const std::string& projectConfiguration);

private:
    GCHeap* mHeap;
    ICompilerListener* mListener;
    CompiledOutput* mLinkerOutput;
    std::optional<std::filesystem::path> mJdkPath;
    std::filesystem::path mProjectPath;
    std::filesystem::path mOutputPath;
    std::filesystem::path mResourcesPath;
    std::optional<std::filesystem::path> mGeneratedWavFile;
    bool mEnableWav;
    bool mShouldDetachJVM;

    bool initSourceFile(SourceFile& sourceFile, FileType fileType, const std::filesystem::path& filePath);

    DISABLE_COPY(Compiler);
};

#endif
