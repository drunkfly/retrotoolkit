#ifndef COMPILER_PROJECT_H
#define COMPILER_PROJECT_H

#include "Compiler/Compression/Compression.h"

class Expr;
class SourceLocation;
class SourceLocationFactory;
class SymbolTable;

class Project
{
public:
    static const char* FileSuffix;
    static const char* DefaultOutputDirectory;

    struct File;

    struct Constant
    {
        SourceLocation* location;
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
        SourceLocation* location;
        std::string name;
        SourceLocation* nameLocation;
        std::optional<std::string> base;
        SourceLocation* baseLocation;
        std::optional<std::string> fileOffset;
        SourceLocation* fileOffsetLocation;
        std::optional<std::string> alignment;
        SourceLocation* alignmentLocation;
        Attachment attachment;
        Compression compression;
        SourceLocation* compressionLocation;
    };

    struct File
    {
        SourceLocation* location;
        std::string name;
        SourceLocation* nameLocation;
        std::optional<std::string> start;
        SourceLocation* startLocation;
        std::optional<std::string> until;
        SourceLocation* untilLocation;
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
            std::optional<std::string> name;
        };

        Type type;
        SourceLocation* location;
        std::optional<std::string> enabled;
        std::vector<File> files;

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

    void load(std::filesystem::path path, SourceLocationFactory* locationFactory);
    void save(std::filesystem::path path, bool createNew = false);

private:
    std::filesystem::path mPath;

    DISABLE_COPY(Project);
};

#endif
