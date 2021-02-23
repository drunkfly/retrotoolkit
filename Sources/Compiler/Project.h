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

class Project
{
public:
    static const char* FileSuffix;

    struct File;

    struct Section
    {
        File* file;
        std::string name;
        std::optional<std::string> base;
        std::optional<std::string> fileOffset;
        std::optional<std::string> alignment;
        Compression compression;
    };

    struct File
    {
        std::string name;
        std::optional<std::string> start;
        std::optional<std::string> until;
        std::vector<std::unique_ptr<Section>> lowerSections;
        std::vector<std::unique_ptr<Section>> upperSections;
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
            Project::File* ref;
        };

        Type type;
        std::optional<bool> enabled;
        std::vector<File> files;
    };

    std::vector<std::unique_ptr<File>> files;
    std::vector<std::unique_ptr<Output>> outputs;

    Project();
    ~Project();

    void load(const std::filesystem::path& path);
    void save(const std::filesystem::path& path, bool createNew = false);

    DISABLE_COPY(Project);
};

#endif
