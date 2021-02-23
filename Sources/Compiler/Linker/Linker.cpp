#include "Linker.h"
#include "Common/GC.h"
#include "Compiler/Tree/SourceLocation.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/Linker/CompiledOutput.h"
#include "Compiler/Linker/ProgramSection.h"
#include "Compiler/Linker/Program.h"
#include "Compiler/Project.h"
#include "Compiler/ExpressionParser.h"
#include "Compiler/CompilerError.h"
#include <sstream>
#include <unordered_set>
#include <vector>

namespace
{
    struct LinkerSection : public GCObject
    {
        ProgramSection* programSection;
        Expr* base;
        Expr* fileOffset;
        Expr* alignment;
    };

    class LinkerFile : public GCObject
    {
    public:
        LinkerFile(SourceLocation* location, const Project::File* file, Program* program, SymbolTable* projectVariables)
            : mLocation(location)
            , mProgram(program)
            , mProjectVariables(projectVariables)
            , mFile(file)
            , mIsResolved(false)
        {
            registerFinalizer();

            mFileStart = tryParseExpression(location, file->start);
            mFileUntil = tryParseExpression(location, file->until);

            mSections.reserve(file->lowerSections.size() + file->upperSections.size());
            for (const auto& it : file->lowerSections)
                addSection(location, it.get());
            mSections.emplace_back(nullptr); // separator between lower/upper sections
            for (const auto& it : file->upperSections)
                addSection(location, it.get());
        }

        const Project::File* file() const { return mFile; }
        bool isResolved() const { return mIsResolved; }

        bool tryResolve()
        {
            // FIXME
            return false;
        }

        void generateCode(CompiledFile* output)
        {
            // FIXME
        }

    private:
        SourceLocation* mLocation;
        Program* mProgram;
        SymbolTable* mProjectVariables;
        const Project::File* mFile;
        std::unordered_set<ProgramSection*> mSectionSet;
        std::vector<LinkerSection*> mSections;
        Expr* mFileStart;
        Expr* mFileUntil;
        bool mIsResolved;

        void addSection(SourceLocation* location, const Project::Section* sectionInfo)
        {
            auto section = mProgram->getOrAddSection(sectionInfo->name);
            if (!mSectionSet.emplace(section).second) {
                std::stringstream ss;
                ss << "section \"" << sectionInfo->name
                    << " is referenced multiple times for file \"" << sectionInfo->file->name << "\".";
                throw CompilerError(location, ss.str());
            }

            auto linkerSection = new (heap()) LinkerSection();
            linkerSection->programSection = section;
            linkerSection->base = tryParseExpression(location, sectionInfo->base);
            linkerSection->fileOffset = tryParseExpression(location, sectionInfo->fileOffset);
            linkerSection->alignment = tryParseExpression(location, sectionInfo->alignment);
            mSections.emplace_back(linkerSection);
        }

        Expr* parseExpression(SourceLocation* location, const std::string& str)
        {
            ExpressionParser parser(heap(), nullptr, nullptr);
            Expr* expr = parser.tryParseExpression(location, str.c_str(), mProjectVariables);
            if (!expr) {
                std::stringstream ss;
                ss << "unable to parse expression \"" << str << "\": " << parser.error();
                throw CompilerError(parser.errorLocation(), ss.str());
            }
            return expr;
        }

        Expr* tryParseExpression(SourceLocation* location, const std::optional<std::string>& str)
        {
            if (!str)
                return nullptr;
            return parseExpression(location, *str);
        }

        DISABLE_COPY(LinkerFile);
    };
}

Linker::Linker(GCHeap* heap, const Project* project, std::string projectConfiguration)
    : mHeap(heap)
    , mProject(project)
    , mProgram(nullptr)
    , mProjectConfiguration(std::move(projectConfiguration))
{
}

Linker::~Linker()
{
}

CompiledOutput* Linker::link(Program* program)
{
    mProgram = program;

    auto projectVariables = new (mHeap) SymbolTable(nullptr);
    mProject->setVariables(projectVariables, mProjectConfiguration);

    auto fileID = new (mHeap) FileID(mProject->path().filename(), mProject->path());
    auto location = new (mHeap) SourceLocation(fileID, 1);

    std::unordered_set<std::string> fileNames;
    std::vector<LinkerFile*> files;

    files.reserve(mProject->files.size());
    for (const auto& file : mProject->files) {
        if (!fileNames.emplace(file->name).second) {
            std::stringstream ss;
            ss << "duplicate file name \"" << file->name << "\".";
            throw CompilerError(location, ss.str());
        }
        files.emplace_back(new (mHeap) LinkerFile(location, file.get(), mProgram, projectVariables));
    }

    for (;;) {
        bool resolvedAll = true;
        bool didResolve = false;

        for (const auto& file : files) {
            if (!file->isResolved()) {
                if (file->tryResolve())
                    didResolve = true;
                else
                    resolvedAll = false;
            }
        }

        if (resolvedAll)
            break;

        if (!didResolve)
            throw CompilerError(location, "resolve error."); // FIXME
    }

    auto output = new (mHeap) CompiledOutput();
    for (const auto& file : files)
        file->generateCode(output->getOrAddFile(file->file()->name));

    return output;
}
