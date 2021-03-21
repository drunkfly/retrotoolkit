#ifndef COMPILER_PROJECT_H
#define COMPILER_PROJECT_H

#include "Common/Common.h"
#include "Compiler/Compression/Compression.h"
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <filesystem>

class Expr;
class SymbolTable;
class SourceLocation;
class GCHeap;

class Project
{
public:
    static const char* FileSuffix;
    static const char* DefaultOutputDirectory;

    struct File;

    struct Constant
    {
        std::string name;
        std::string value;
    };

    struct Configuration
    {
        std::string name;
        std::vector<Constant> constants;
    };

    struct Section
    {
        enum Attachment
        {
            Default,
            Lower,
            Upper,
        };

        File* file;
        std::string name;
        std::optional<std::string> base;
        std::optional<std::string> fileOffset;
        std::optional<std::string> alignment;
        Attachment attachment;
        Compression compression;
    };

    struct File
    {
        std::string name;
        std::optional<std::string> start;
        std::optional<std::string> until;
        std::vector<std::unique_ptr<Section>> sections;
    };

    struct Output
    {
        enum Type
        {
            ZXSpectrumTAP,
            ZXSpectrumTRD,
        };

        struct File
        {
            SourceLocation* location;
            std::optional<std::string> ref;
            std::optional<std::string> basic;
        };

        Type type;
        std::optional<std::string> enabled;
        std::vector<File> files;
        SourceLocation* location;

        bool isEnabled(SymbolTable* symbolTable) const;
    };

    std::vector<Constant> constants;
    std::vector<std::unique_ptr<Configuration>> configurations;
    std::vector<std::unique_ptr<File>> files;
    std::vector<std::unique_ptr<Output>> outputs;
    std::optional<std::string> outputDirectory;

    Project();
    ~Project();

    const std::filesystem::path& path() const { return mPath; }

    void setVariables(SymbolTable* symbolTable, const std::string& configuration) const;

    void load(std::filesystem::path path);
    void save(std::filesystem::path path, bool createNew = false);

private:
    std::filesystem::path mPath;

    DISABLE_COPY(Project);
};

#endif
