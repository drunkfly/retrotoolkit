#include "Linker.h"
#include "Common/GC.h"
#include "Compiler/Tree/Expr.h"
#include "Compiler/Tree/SourceLocation.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/Linker/CompiledOutput.h"
#include "Compiler/Linker/CodeEmitterCompressed.h"
#include "Compiler/Linker/ProgramSection.h"
#include "Compiler/Linker/Program.h"
#include "Compiler/Compression/Compressor.h"
#include "Compiler/Project.h"
#include "Compiler/ExpressionParser.h"
#include "Compiler/CompilerError.h"
#include <sstream>
#include <unordered_set>
#include <vector>
#include <algorithm>

#define DEBUG_LINKER 1

#if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace
{
    struct LinkerSection : public GCObject
    {
        ProgramSection* programSection;
        Project::Section::Attachment attachment;
        Compression compression;
        std::unique_ptr<CodeEmitter> code;
        Expr* base;
        Expr* fileOffset;
        Expr* alignment;
        size_t uncompressedSize;
        std::optional<size_t> resolvedSize;
        std::optional<size_t> resolvedBase;
        std::optional<size_t> resolvedFileOffset;
        bool autoFileOffset;
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

            mSections.reserve(file->sections.size());
            for (const auto& it : file->sections)
                addSection(location, it.get());

            // we can easily calculate size for all uncompressed sections

            for (auto section : mSections) {
                if (section->compression == Compression::None) {
                    section->resolvedSize = section->uncompressedSize;
                  #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                    { std::stringstream ss;
                    ss << "resolved size " << *section->resolvedSize << " for \"" << section->programSection->name()
                        << "\" in file \"" << file->name << "\".\n";
                    OutputDebugStringA(ss.str().c_str()); }
                  #endif
                }
            }

            // resolve addresses for all sections with known base

            bool hasSectionWithKnownBase = false;
            for (auto section : mSections) {
                if (section->base) {
                    section->resolvedBase = section->base->evaluateUnsignedWord(nullptr);
                  #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                    { std::stringstream ss;
                    ss << "resolved base 0x" << std::hex << *section->resolvedBase
                        << " for \"" << section->programSection->name()
                        << "\" in file \"" << file->name << "\".\n";
                    OutputDebugStringA(ss.str().c_str()); }
                  #endif

                    section->programSection->resolveLabels(*section->resolvedBase);
                  #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                    { std::stringstream ss;
                    ss << "resolved labels in \"" << section->programSection->name()
                        << "\" in file \"" << file->name << "\".\n";
                    OutputDebugStringA(ss.str().c_str()); }
                  #endif

                    if (section->fileOffset && !section->autoFileOffset) {
                        section->resolvedFileOffset = section->fileOffset->evaluateUnsignedWord(nullptr);
                      #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                        { std::stringstream ss;
                        ss << "resolved file offset 0x" << std::hex << *section->resolvedFileOffset
                            << " for \"" << section->programSection->name()
                            << "\" in file \"" << file->name << "\".\n";
                        OutputDebugStringA(ss.str().c_str()); }
                      #endif
                        hasSectionWithKnownBase = true;
                    }

                    if (section->alignment) {
                        size_t alignment = section->alignment->evaluateUnsignedWord(nullptr);
                        if (alignment == 0) {
                            std::stringstream ss;
                            ss << "section \"" << section->programSection->name()
                                << "\" has invalid alignment in file \"" << file->name << "\".";
                            throw CompilerError(location, ss.str());
                        } else if ((section->resolvedBase.value() % alignment) != 0) {
                            std::stringstream ss;
                            ss << "conflicting base and alignment for section \""
                                << section->programSection->name() << "\" in file \"" << file->name << "\".";
                            throw CompilerError(location, ss.str());
                        }
                    }
                }
            }

            // if neither start, nor end of file is specified, there should be at least one section with known base

            if (!mFileStart && !mFileUntil && !hasSectionWithKnownBase) {
                std::stringstream ss;
                ss << "unable to resolve addresses in file \"" << file->name << "\".";
                throw CompilerError(location, ss.str());
            }

            // convert all "lower" sections at the beginning of file to "upper" if file start is unspecified

            if (!mFileStart) {
                for (auto section : mSections) {
                    if (section->resolvedFileOffset)
                        break;
                    if (section->attachment == Project::Section::Upper)
                        break;
                    section->attachment = Project::Section::Upper;
                }
            }

            // convert all "upper" sections at the end of file to "lower" if file end is unspecified

            if (!mFileUntil) {
                for (auto section : mSections) {
                    if (section->resolvedFileOffset)
                        break;
                    if (section->attachment != Project::Section::Upper)
                        break;
                    section->attachment = Project::Section::Lower;
                }
            }

            // resolve addresses for sections at start of file while we can

            if (mFileStart) {
                size_t address = mFileStart->evaluateUnsignedWord(nullptr);
                resolveSectionsFrom(address, 0);
            }

            // resolve addresses for sections at end of file while we can

            if (mFileUntil) {
                size_t address = mFileUntil->evaluateUnsignedWord(nullptr);
                resolveSectionsTo(address, mSections.size());
            }
        }

        const Project::File* file() const { return mFile; }

        LinkerSection* firstUnresolvedSection() const
        {
            for (auto section : mSections) {
                if (!section->resolvedFileOffset || !section->resolvedSize || !section->code)
                    return section;
            }
            return nullptr;
        }

        bool tryResolve(bool& didResolve, std::unique_ptr<CompilerError>& resolveError)
        {
            if (mIsResolved)
                return true;

            didResolve = false;

            // Try to compress some sections

            for (auto section : mSections) {
                if (section->resolvedSize || !section->resolvedBase)
                    continue;

                if (section->compression == Compression::None) {
                    throw CompilerError(mLocation,
                        "internal compiler error: unexpected uncompressed section with unresolved size.");
                }

                auto compressor = Compressor::create(mLocation, section->compression);
                auto code = std::make_unique<CodeEmitterCompressed>(std::move(compressor));

                if (section->programSection->emitCode(code.get(), section->resolvedBase.value(), resolveError)) {
                  #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                    { std::stringstream ss;
                    ss << "generated code for section \"" << section->programSection->name()
                        << "\" in file \"" << file()->name << "\".\n";
                    OutputDebugStringA(ss.str().c_str()); }
                  #endif

                    code->compress();

                    section->resolvedSize = code->compressedSize();
                  #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                    { std::stringstream ss;
                    ss << "resolved size " << *section->resolvedSize << " for \"" << section->programSection->name()
                        << "\" in file \"" << file()->name << "\".\n";
                    OutputDebugStringA(ss.str().c_str()); }
                  #endif

                    section->code = std::move(code);
                    didResolve = true;
                }
            }

            // Try to resolve still-unresolved sections

            bool hasUnresolved = false;
            size_t n = mSections.size();
            for (size_t i = 0; i < n; i++) {
                auto section = mSections[i];
                if (!section->resolvedFileOffset) {
                    hasUnresolved = true;
                    continue;
                }

                if (!section->code && section->compression == Compression::None) {
                    auto code = std::make_unique<CodeEmitterUncompressed>();
                    if (!section->programSection->emitCode(code.get(), section->resolvedBase.value(), resolveError)) {
                        hasUnresolved = true;
                        continue;
                    } else {
                      #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                        { std::stringstream ss;
                        ss << "generated code for section \"" << section->programSection->name()
                            << "\" in file \"" << file()->name << "\".\n";
                        OutputDebugStringA(ss.str().c_str()); }
                      #endif

                        if (!section->resolvedSize) {
                            section->resolvedSize = code->size();
                          #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                            { std::stringstream ss;
                            ss << "resolved size " << *section->resolvedSize
                                << " for \"" << section->programSection->name()
                                << "\" in file \"" << file()->name << "\".\n";
                            OutputDebugStringA(ss.str().c_str()); }
                          #endif
                        } else if (*section->resolvedSize != code->size()) {
                            std::stringstream ss;
                            ss << "internal compiler error: size of generated code for section \""
                                << section->programSection->name() << "\" in file \"" << file()->name
                                << "\" differs from resolved size ("
                                << *section->resolvedSize << " != " << code->size() << ").";
                            throw CompilerError(mLocation, ss.str());
                        }

                        section->code = std::move(code);
                        didResolve = true;
                    }
                }

                size_t off = *section->resolvedFileOffset;
                didResolve = resolveSectionsTo(off, i) || didResolve;

                if (!section->resolvedSize)
                    hasUnresolved = true;
                else
                    didResolve = resolveSectionsFrom(off + *section->resolvedSize, i + 1) || didResolve;
            }

            // Did we resolve all?

            if (!hasUnresolved) {
                mIsResolved = true;
                return true;
            }

            return false;
        }

        void generateCode(CompiledFile* output)
        {
            if (mSections.empty())
                return;

            std::sort(mSections.begin(), mSections.end(), [](LinkerSection* a, LinkerSection* b) {
                    return (a->resolvedFileOffset.value() < b->resolvedFileOffset.value());
                });

            size_t startAddress = 0;
            if (mFileStart)
                startAddress = mFileStart->evaluateUnsignedWord(nullptr);
            else
                startAddress = mSections[0]->resolvedFileOffset.value();

            size_t endAddress = 0;
            if (mFileUntil) {
                endAddress = mFileUntil->evaluateUnsignedWord(nullptr);
                if (mFileStart && startAddress >= endAddress) {
                    std::stringstream ss;
                    ss << "file \"" << mFile->name << "\"has invalid bounds.";
                    throw CompilerError(mLocation, ss.str());
                }
            }

            size_t offset = startAddress;
            for (auto section : mSections) {
                size_t targetOffset = section->resolvedFileOffset.value();
                if (targetOffset < startAddress) {
                    std::stringstream ss;
                    ss << "section \"" << section->programSection->name()
                        << "\" is out of bounds in file \"" << mFile->name << "\".";
                    throw CompilerError(mLocation, ss.str());
                }
                if (targetOffset < offset) {
                    std::stringstream ss;
                    ss << "section \"" << section->programSection->name()
                        << "\" is overlapping with previous section in file \"" << mFile->name << "\".";
                    throw CompilerError(mLocation, ss.str());
                }

                while (targetOffset > offset) {
                    output->emitByte(nullptr, 0);
                    ++offset;
                }

                if (section->code)
                    section->code->copyTo(output);
                else {
                    if (section->compression != Compression::None) {
                        std::stringstream ss;
                        ss << "internal compiler error: no code was generated for compressed section \""
                            << section->programSection->name() << "\" in file \"" << mFile->name << "\".";
                        throw CompilerError(mLocation, ss.str());
                    }

                    std::unique_ptr<CompilerError> resolveError;
                    if (!section->programSection->emitCode(output, offset, resolveError)) {
                      #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                        { std::stringstream ss;
                        ss << "generated code for section \"" << section->programSection->name()
                            << "\" in file \"" << file()->name << "\".\n";
                        OutputDebugStringA(ss.str().c_str()); }
                      #endif
                    } else {
                        if (resolveError)
                            throw* resolveError;

                        std::stringstream ss;
                        ss << "unable to emit code for section \"" << section->programSection->name()
                            << "\" in file \"" << mFile->name << "\".";
                        throw CompilerError(mLocation, ss.str());
                    }
                }

                offset += section->resolvedSize.value();
            }

            output->setLoadAddress(startAddress);
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
                    << "\" is referenced multiple times for file \"" << sectionInfo->file->name << "\".";
                throw CompilerError(location, ss.str());
            }

            bool autoOffset = (sectionInfo->fileOffset && *sectionInfo->fileOffset == "auto");

            auto linkerSection = new (heap()) LinkerSection();
            linkerSection->programSection = section;
            linkerSection->base = tryParseExpression(location, sectionInfo->base);
            linkerSection->fileOffset = (autoOffset ? nullptr : tryParseExpression(location, sectionInfo->fileOffset));
            linkerSection->alignment = tryParseExpression(location, sectionInfo->alignment);
            linkerSection->uncompressedSize = section->calculateSizeInBytes();
            linkerSection->attachment = sectionInfo->attachment;
            linkerSection->compression = sectionInfo->compression;
            linkerSection->autoFileOffset = autoOffset;
            mSections.emplace_back(linkerSection);

            if (linkerSection->fileOffset && !linkerSection->base) {
                std::stringstream ss;
                ss << "section \"" << sectionInfo->name << "\" has file offset without base address.";
                throw CompilerError(location, ss.str());
            }
        }

        bool resolveSectionsFrom(size_t address, size_t i)
        {
            bool resolvedSomething = false;

            size_t n = mSections.size();
            for (; i < n; i++) {
                auto section = mSections[i];
                if (section->attachment == Project::Section::Attachment::Upper || section->resolvedFileOffset)
                    break;

                resolvedSomething = true;

                if (section->alignment) {
                    size_t alignment = section->alignment->evaluateUnsignedWord(nullptr);
                    if (alignment == 0) {
                        std::stringstream ss;
                        ss << "section \"" << section->programSection->name()
                            << "\" has invalid alignment in file \"" << mFile->name << "\".";
                        throw CompilerError(mLocation, ss.str());
                    }
                    address += alignment - 1;
                    address /= alignment;
                    address *= alignment;
                }

                section->resolvedFileOffset = address;
              #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                { std::stringstream ss;
                ss << "resolved file offset 0x" << std::hex << *section->resolvedFileOffset
                    << " for \"" << section->programSection->name() << "\" in file \"" << file()->name << "\".\n";
                OutputDebugStringA(ss.str().c_str()); }
              #endif

                if (!section->resolvedBase) {
                    section->resolvedBase = address;
                  #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                    { std::stringstream ss;
                    ss << "resolved base 0x" << std::hex << *section->resolvedBase
                        << " for \"" << section->programSection->name() << "\" in file \"" << file()->name << "\".\n";
                    OutputDebugStringA(ss.str().c_str()); }
                  #endif

                    section->programSection->resolveLabels(address);
                  #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                    { std::stringstream ss;
                    ss << "resolved labels in \"" << section->programSection->name()
                        << "\" in file \"" << file()->name << "\".\n";
                    OutputDebugStringA(ss.str().c_str()); }
                  #endif
                }

                if (!section->resolvedSize)
                    break;

                address += *section->resolvedSize;
            }

            return resolvedSomething;
        }

        bool resolveSectionsTo(size_t address, size_t i)
        {
            bool resolvedSomething = false;

            while (i-- > 0) {
                auto section = mSections[i];
                if (section->attachment == Project::Section::Attachment::Lower || section->resolvedFileOffset)
                    break;
                if (!section->resolvedSize)
                    break;

                resolvedSomething = true;
                address -= *section->resolvedSize;

                if (section->alignment) {
                    size_t alignment = section->alignment->evaluateUnsignedWord(nullptr);
                    if (alignment == 0) {
                        std::stringstream ss;
                        ss << "section \"" << section->programSection->name()
                            << "\" has invalid alignment in file \"" << mFile->name << "\".";
                        throw CompilerError(mLocation, ss.str());
                    }
                    address -= address % alignment;
                }

                section->resolvedFileOffset = address;
              #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                { std::stringstream ss;
                ss << "resolved file offset 0x" << std::hex << *section->resolvedFileOffset
                    << " for \"" << section->programSection->name()
                    << "\" in file \"" << file()->name << "\".\n";
                OutputDebugStringA(ss.str().c_str()); }
              #endif

                if (!section->resolvedBase) {
                    section->resolvedBase = address;
                  #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                    { std::stringstream ss;
                    ss << "resolved base 0x" << std::hex << *section->resolvedBase
                        << " for \"" << section->programSection->name()
                        << "\" in file \"" << file()->name << "\".\n";
                    OutputDebugStringA(ss.str().c_str()); }
                  #endif

                    section->programSection->resolveLabels(address);
                  #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                    { std::stringstream ss;
                    ss << "resolved labels in \"" << section->programSection->name()
                        << "\" in file \"" << file()->name << "\".\n";
                    OutputDebugStringA(ss.str().c_str()); }
                  #endif
                }
            }

            return resolvedSomething;
        }

        Expr* parseExpression(SourceLocation* location, const std::string& str)
        {
            ExpressionParser parser(heap(), nullptr, nullptr, nullptr);
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
        std::unique_ptr<CompilerError> resolveError;

        for (const auto& file : files) {
            if (!file->tryResolve(didResolve, resolveError))
                resolvedAll = false;
        }

        if (resolvedAll)
            break;

        if (!didResolve) {
            if (resolveError)
                throw *resolveError;

            for (const auto& file : files) {
                auto section = file->firstUnresolvedSection();
                if (section) {
                    std::stringstream ss;
                    ss << "unable to resolve section \"" << section->programSection->name()
                        << "\" in file \"" << file->file()->name << "\".";
                    throw CompilerError(location, ss.str());
                }
            }

            throw CompilerError(location, "unable to resolve section.");
        }
    }

    auto output = new (mHeap) CompiledOutput();
    for (const auto& file : files)
        file->generateCode(output->getOrAddFile(file->file()->name));

    return output;
}
