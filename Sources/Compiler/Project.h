#ifndef COMPILER_PROJECT_H
#define COMPILER_PROJECT_H

#include "Compiler/Compression/Compression.h"

class Expr;
class SourceLocation;
class SourceLocationFactory;
class SymbolTable;
class Value;
class ISectionResolver;
class Program;
class SpectrumSnapshotWriter;

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
        std::optional<std::string> condition;
        SourceLocation* conditionLocation;
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
            ZXSpectrumZ80,
            PC,
        };

        struct File
        {
            SourceLocation* location;
            std::optional<std::string> ref;
            std::optional<std::string> refBasic;
            std::optional<std::string> name;
        };

        struct Z80
        {
            struct Value
            {
                SourceLocation* location = nullptr;
                std::optional<std::string> value;

                Expr* parseExpression(Program* program) const;

                uint8_t evaluateByte(Program* program, ISectionResolver* sectionResolver) const;
                uint16_t evaluateWord(Program* program, ISectionResolver* sectionResolver) const;
                bool evaluateBool(Program* program, ISectionResolver* sectionResolver) const;
                ::Value evaluateValue(Program* program, ISectionResolver* sectionResolver) const;
            };

            Value format;
            Value machine;
            Value a;
            Value f;
            Value bc;
            Value hl;
            Value de;
            Value shadowA;
            Value shadowF;
            Value shadowBC;
            Value shadowHL;
            Value shadowDE;
            Value pc;
            Value sp;
            Value iy;
            Value ix;
            Value i;
            Value r;
            Value borderColor;
            Value interruptMode;
            Value port7FFD;
            Value portFFFD;
            Value port1FFD;
            Value interruptsEnabled;

            void initWriter(Program* program, ISectionResolver* sectionResolver, SpectrumSnapshotWriter* writer) const;
        };

        Type type;
        SourceLocation* location;
        std::optional<std::string> enabled;
        std::vector<File> files;
        std::unique_ptr<Z80> z80;

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
