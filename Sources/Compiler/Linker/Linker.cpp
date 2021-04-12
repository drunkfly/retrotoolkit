#include "Linker.h"
#include "Common/GC.h"
#include "Compiler/Tree/Expr.h"
#include "Compiler/Tree/SourceLocation.h"
#include "Compiler/Tree/Symbol.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/Linker/CompiledOutput.h"
#include "Compiler/Linker/CodeEmitterCompressed.h"
#include "Compiler/Linker/ProgramSection.h"
#include "Compiler/Linker/Program.h"
#include "Compiler/Linker/DebugInformation.h"
#include "Compiler/Assembler/Label.h"
#include "Compiler/Compression/Compressor.h"
#include "Compiler/Project.h"
#include "Compiler/ExpressionParser.h"
#include "Compiler/CompilerError.h"

//#define DEBUG_LINKER 1

#if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace
{
    struct LinkerSection : public GCObject
    {
        SourceLocation* location;
        ProgramSection* programSection;
        Project::Section::Attachment attachment;
        Compression compression;
        SourceLocation* compressionLocation;
        std::unique_ptr<CodeEmitter> code;
        CodeEmitterCompressed* compressedCode;
        Expr* base;
        Expr* fileOffset;
        Expr* alignment;
        std::optional<size_t> resolvedSize;
        std::optional<size_t> resolvedBase;
        std::optional<size_t> resolvedFileOffset;
        bool labelsResolved;
        bool autoFileOffset;
    };

    class LinkerFile : public GCObject, public ISectionResolver
    {
    public:
        LinkerFile(std::unordered_set<std::string>& usedSections, const Project::File* file, Program* program)
            : mProgram(program)
            , mFile(file)
            , mDebugInfo(new DebugInformation())
            , mIsResolved(false)
        {
            registerFinalizer();

            mFileStart = tryParseExpression(file->startLocation, file->start);
            mFileUntil = tryParseExpression(file->untilLocation, file->until);

            mSections.reserve(file->sections.size());
            for (const auto& it : file->sections)
                addSection(usedSections, it.get());

            // try calculate size for all uncompressed sections

            for (auto section : mSections) {
                if (section->compression == Compression::None) {
                    size_t size = 0;
                    bool sizeResolved = false;
                    if (section->resolvedBase.has_value()) {
                        std::unique_ptr<CompilerError> error;
                        size_t address = *section->resolvedBase;
                        if (!section->programSection->resolveLabels(address, this, error))
                            section->programSection->unresolveLabels();
                        else {
                            section->labelsResolved = true;
                            size = address - *section->resolvedBase;
                            sizeResolved = true;
                          #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                            { std::stringstream ss;
                            ss << "resolved labels in \"" << section->programSection->name()
                                << "\" in file \"" << file()->name << "\".\n";
                            OutputDebugStringA(ss.str().c_str()); }
                          #endif
                        }
                    }

                    if (!sizeResolved) {
                        std::unique_ptr<CompilerError> error;
                        if (!section->programSection->calculateSizeInBytes(size, this, error))
                            continue;
                    } else {
                      #ifndef NDEBUG
                        std::unique_ptr<CompilerError> error;
                        size_t calculatedSize = 0;
                        if (!section->programSection->calculateSizeInBytes(calculatedSize, this, error)) {
                            assert(false);
                            throw* error;
                        }
                        if (calculatedSize != size) {
                            assert(false);
                            std::stringstream ss;
                            ss << "internal compiler error: calculated size of section \""
                                << section->programSection->name() << "\" in file \"" << file->name
                                << "\" differs from resolved size ("
                                << calculatedSize << " != " << size << ").";
                            throw CompilerError(section->location, ss.str());
                        }
                      #endif
                    }

                    section->resolvedSize = size;
                  #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                    { std::stringstream ss;
                    ss << "resolved size " << *section->resolvedSize << " for \"" << section->programSection->name()
                        << "\" in file \"" << file->name << "\".\n";
                    OutputDebugStringA(ss.str().c_str()); }
                  #endif
                }
            }

            // resolve addresses for all sections with known base

            bool hasSectionWithKnownFileOffset = false;
            for (auto section : mSections) {
                if (section->base) {
                    section->resolvedBase = section->base->evaluateUnsignedWord(nullptr, this);
                  #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                    { std::stringstream ss;
                    ss << "resolved base 0x" << std::hex << *section->resolvedBase
                        << " for \"" << section->programSection->name()
                        << "\" in file \"" << file->name << "\".\n";
                    OutputDebugStringA(ss.str().c_str()); }
                  #endif

                    if (section->fileOffset && !section->autoFileOffset) {
                        section->resolvedFileOffset =
                            section->fileOffset->evaluateUnsignedWord(nullptr, this);
                      #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                        { std::stringstream ss;
                        ss << "resolved file offset 0x" << std::hex << *section->resolvedFileOffset
                            << " for \"" << section->programSection->name()
                            << "\" in file \"" << file->name << "\".\n";
                        OutputDebugStringA(ss.str().c_str()); }
                      #endif
                        hasSectionWithKnownFileOffset = true;
                    }

                    if (section->alignment) {
                        size_t alignment = section->alignment->evaluateUnsignedWord(nullptr, this);
                        if (alignment == 0) {
                            std::stringstream ss;
                            ss << "section \"" << section->programSection->name()
                                << "\" has invalid alignment in file \"" << file->name << "\".";
                            throw CompilerError(section->alignment->location(), ss.str());
                        } else if ((section->resolvedBase.value() % alignment) != 0) {
                            std::stringstream ss;
                            ss << "conflicting base and alignment for section \""
                                << section->programSection->name() << "\" in file \"" << file->name << "\".";
                            throw CompilerError(section->alignment->location(), ss.str());
                        }
                    }
                }
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
                size_t address = mFileStart->evaluateUnsignedWord(nullptr, this);
                resolveSectionsFrom(address, 0);
            }

            // resolve addresses for sections at end of file while we can

            if (mFileUntil) {
                size_t address = mFileUntil->evaluateUnsignedWord(nullptr, this);
                resolveSectionsTo(address, mSections.size());
            }

            // if neither start, nor end of file is specified, there should be at least one section with known base

            if (!mFileStart && !mFileUntil && !hasSectionWithKnownFileOffset) {
                bool found = false;
                for (auto section : mSections) {
                    if (section->resolvedBase && !section->fileOffset && !section->autoFileOffset) {
                        section->resolvedFileOffset = *section->resolvedBase;
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    for (auto section : mSections) {
                        if (section->resolvedBase && section->autoFileOffset) {
                            section->resolvedFileOffset = *section->resolvedBase;
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        std::stringstream ss;
                        ss << "unable to resolve addresses in file \"" << file->name
                            << "\": there is no section with known base and neither start, "
                                "nor end addresses for file were not specified.";
                        throw CompilerError(file->location, ss.str());
                    }
                }
            }
        }

        const Project::File* file() const { return mFile; }

        std::unique_ptr<DebugInformation> takeDebugInfo()
        {
            std::unique_ptr<DebugInformation> info{std::move(mDebugInfo)};
            return info;
        }

        LinkerSection* firstUnresolvedSection() const
        {
            for (auto section : mSections) {
                if (!section->resolvedFileOffset || !section->resolvedSize || !section->labelsResolved || !section->code)
                    return section;
            }
            return nullptr;
        }

        bool tryResolve(bool& didResolve, std::unique_ptr<CompilerError>& resolveError)
        {
            if (mIsResolved)
                return true;

            bool hasUnresolved = false;
            didResolve = false;

            // Try to resolve size and labels for sections

            for (auto section : mSections) {
                if (section->compression == Compression::None && !section->resolvedSize) {
                    size_t size = 0;
                    bool sizeResolved = false;
                    if (section->resolvedBase.has_value()) {
                        std::unique_ptr<CompilerError> error;
                        size_t address = *section->resolvedBase;
                        if (!section->programSection->resolveLabels(address, this, error))
                            section->programSection->unresolveLabels();
                        else {
                            section->labelsResolved = true;
                            size = address - *section->resolvedBase;
                            sizeResolved = true;
                          #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                            { std::stringstream ss;
                            ss << "resolved labels in \"" << section->programSection->name()
                                << "\" in file \"" << file()->name << "\".\n";
                            OutputDebugStringA(ss.str().c_str()); }
                          #endif
                        }
                    }

                    if (!sizeResolved)
                        sizeResolved = section->programSection->calculateSizeInBytes(size, this, resolveError);
                    else {
                      #ifndef NDEBUG
                        size_t calculatedSize = 0;
                        if (!section->programSection->calculateSizeInBytes(calculatedSize, this, resolveError)) {
                            assert(false);
                            throw* resolveError;
                        }
                        if (calculatedSize != size) {
                            assert(false);
                            std::stringstream ss;
                            ss << "internal compiler error: calculated size of section \""
                                << section->programSection->name() << "\" in file \"" << file()->name
                                << "\" differs from resolved size ("
                                << calculatedSize << " != " << size << ").";
                            throw CompilerError(section->location, ss.str());
                        }
                      #endif
                    }

                    if (!sizeResolved)
                        hasUnresolved = true;
                    else {
                        section->resolvedSize = size;
                      #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                        { std::stringstream ss;
                        ss << "resolved size " << *section->resolvedSize << " for \"" << section->programSection->name()
                            << "\" in file \"" << file()->name << "\".\n";
                        OutputDebugStringA(ss.str().c_str()); }
                      #endif
                        didResolve = true;
                    }
                }

                if (!section->labelsResolved && section->resolvedBase) {
                    size_t address = *section->resolvedBase;
                    if (!section->programSection->resolveLabels(address, this, resolveError)) {
                        section->programSection->unresolveLabels();
                        hasUnresolved = true;
                    } else {
                        section->labelsResolved = true;
                      #if defined(_WIN32) && defined(DEBUG_LINKER) && !defined(NDEBUG)
                        { std::stringstream ss;
                        ss << "resolved labels in \"" << section->programSection->name()
                            << "\" in file \"" << file()->name << "\".\n";
                        OutputDebugStringA(ss.str().c_str()); }
                      #endif
                        didResolve = true;
                    }
                }
            }

            // Try to compress some sections

            for (auto section : mSections) {
                if (section->compression == Compression::None)
                    continue;
                if (section->resolvedSize)
                    continue;

                int64_t baseAddress;
                if (section->resolvedBase)
                    baseAddress = *section->resolvedBase;
                else {
                    if (!section->programSection->canEmitCodeWithoutBaseAddress(this))
                        continue;
                    baseAddress = 0;
                }

                auto compressor = Compressor::create(section->compressionLocation, section->compression);
                auto code = std::make_unique<CodeEmitterCompressed>(std::move(compressor));

                if (section->programSection->emitCode(code.get(), baseAddress, this, resolveError)) {
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

                    section->compressedCode = code.get();
                    section->code = std::move(code);
                    didResolve = true;
                }
            }

            // Try to resolve still-unresolved sections

            size_t n = mSections.size();
            for (size_t i = 0; i < n; i++) {
                auto section = mSections[i];
                if (!section->resolvedFileOffset || !section->labelsResolved) {
                    hasUnresolved = true;
                    continue;
                }

                if (!section->code && section->compression == Compression::None) {
                    auto code = std::make_unique<CodeEmitterUncompressed>();
                    if (!section->programSection->emitCode(code.get(),
                            section->resolvedBase.value(), this, resolveError))
                        hasUnresolved = true;
                    else {
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
                            throw CompilerError(section->location, ss.str());
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
                startAddress = mFileStart->evaluateUnsignedWord(nullptr, this);
            else
                startAddress = mSections[0]->resolvedFileOffset.value();

            size_t endAddress = 0;
            if (mFileUntil) {
                endAddress = mFileUntil->evaluateUnsignedWord(nullptr, this);
                if (mFileStart && startAddress >= endAddress) {
                    std::stringstream ss;
                    ss << "file \"" << mFile->name << "\"has invalid bounds.";
                    throw CompilerError(output->location(), ss.str());
                }
            }

            size_t offset = startAddress;
            for (auto section : mSections) {
                size_t targetOffset = section->resolvedFileOffset.value();
                if (targetOffset < startAddress) {
                    std::stringstream ss;
                    ss << "section \"" << section->programSection->name()
                        << "\" is out of bounds in file \"" << mFile->name << "\".";
                    throw CompilerError(section->location, ss.str());
                }
                if (targetOffset < offset) {
                    std::stringstream ss;
                    ss << "section \"" << section->programSection->name()
                        << "\" is overlapping with previous section in file \"" << mFile->name << "\".";
                    throw CompilerError(section->location, ss.str());
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
                        throw CompilerError(section->location, ss.str());
                    }

                    std::unique_ptr<CompilerError> resolveError;
                    if (section->programSection->emitCode(output, offset, this, resolveError)) {
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
                        throw CompilerError(section->location, ss.str());
                    }
                }

                size_t size = section->resolvedSize.value();
                offset += size;

                if (mFileUntil && size > 0 && offset > endAddress) {
                    std::stringstream ss;
                    ss << "section \"" << section->programSection->name()
                        << "\" is out of bounds in file \"" << mFile->name << "\".";
                    throw CompilerError(section->location, ss.str());
                }
            }

            // FIXME: how system handles sections with wrong order in project file (regarding base address)?

            output->setLoadAddress(startAddress);
        }

        bool isValidSectionName(const std::string& name) const override
        {
            return mSectionsByName.find(name) != mSectionsByName.end();
        }

    private:
        Program* mProgram;
        const Project::File* mFile;
        std::unique_ptr<DebugInformation> mDebugInfo;
        std::unordered_set<ProgramSection*> mSectionSet;
        std::vector<LinkerSection*> mSections;
        std::unordered_map<std::string, LinkerSection*> mSectionsByName;
        Expr* mFileStart;
        Expr* mFileUntil;
        bool mIsResolved;

        void addSection(std::unordered_set<std::string>& usedSections, const Project::Section* sectionInfo)
        {
            auto section = mProgram->getOrAddSection(sectionInfo->name);
            if (!mSectionSet.emplace(section).second) {
                std::stringstream ss;
                ss << "section \"" << sectionInfo->name
                    << "\" is referenced multiple times for file \"" << sectionInfo->file->name << "\".";
                throw CompilerError(sectionInfo->location, ss.str());
            }

            if (!usedSections.emplace(sectionInfo->name).second)
                section = section->clone();

            bool autoOffset = (sectionInfo->fileOffset && *sectionInfo->fileOffset == "auto");

            auto linkerSection = new (heap()) LinkerSection();
            linkerSection->location = sectionInfo->location;
            linkerSection->programSection = section;
            linkerSection->base = tryParseExpression(sectionInfo->baseLocation, sectionInfo->base);
            linkerSection->compressedCode = nullptr;
            linkerSection->fileOffset =
                (autoOffset ? nullptr : tryParseExpression(sectionInfo->fileOffsetLocation, sectionInfo->fileOffset));
            linkerSection->alignment = tryParseExpression(sectionInfo->alignmentLocation, sectionInfo->alignment);
            linkerSection->attachment = sectionInfo->attachment;
            linkerSection->compression = sectionInfo->compression;
            linkerSection->compressionLocation = sectionInfo->compressionLocation;
            linkerSection->labelsResolved = false;
            linkerSection->autoFileOffset = autoOffset;
            mSections.emplace_back(linkerSection);

            mSectionsByName[sectionInfo->name] = linkerSection;

            if (linkerSection->fileOffset && !linkerSection->base) {
                std::stringstream ss;
                ss << "section \"" << sectionInfo->name << "\" has file offset without base address.";
                throw CompilerError(sectionInfo->nameLocation, ss.str());
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
                    size_t alignment = section->alignment->evaluateUnsignedWord(nullptr, this);
                    if (alignment == 0) {
                        std::stringstream ss;
                        ss << "section \"" << section->programSection->name()
                            << "\" has invalid alignment in file \"" << mFile->name << "\".";
                        throw CompilerError(section->alignment->location(), ss.str());
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
                }

                if (section->compressedCode)
                    section->compressedCode->setSectionBase(address);

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
                    size_t alignment = section->alignment->evaluateUnsignedWord(nullptr, this);
                    if (alignment == 0) {
                        std::stringstream ss;
                        ss << "section \"" << section->programSection->name()
                            << "\" has invalid alignment in file \"" << mFile->name << "\".";
                        throw CompilerError(section->alignment->location(), ss.str());
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
                }

                if (section->compressedCode)
                    section->compressedCode->setSectionBase(address);
            }

            return resolvedSomething;
        }

        Expr* parseExpression(SourceLocation* location, const std::string& str)
        {
            ExpressionParser parser(heap(), nullptr, nullptr, nullptr);
            Expr* expr = parser.tryParseExpression(location, str.c_str(), mProgram->projectVariables());
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

        bool tryResolveSectionAddress(const std::string& sectionName, uint64_t& value) const override
        {
            auto it = mSectionsByName.find(sectionName);
            if (it == mSectionsByName.end())
                return false;

            LinkerSection* section = it->second;
            if (!section->resolvedFileOffset.has_value())
                return false;

            value = section->resolvedFileOffset.value();
            return true;
        }

        bool tryResolveSectionBase(const std::string& sectionName, uint64_t& value) const override
        {
            auto it = mSectionsByName.find(sectionName);
            if (it == mSectionsByName.end())
                return false;

            LinkerSection* section = it->second;
            if (!section->resolvedBase.has_value())
                return false;

            value = section->resolvedBase.value();
            return true;
        }

        bool tryResolveSectionSize(const std::string& sectionName, uint64_t& value) const override
        {
            auto it = mSectionsByName.find(sectionName);
            if (it == mSectionsByName.end())
                return false;

            LinkerSection* section = it->second;
            if (!section->resolvedSize.has_value())
                return false;

            value = section->resolvedSize.value();
            return true;
        }

        DISABLE_COPY(LinkerFile);
    };
}

Linker::Linker(GCHeap* heap, const Project* project)
    : mHeap(heap)
    , mProject(project)
    , mProgram(nullptr)
{
}

Linker::~Linker()
{
}

CompiledOutput* Linker::link(Program* program)
{
    mProgram = program;

    auto output = new (mHeap) CompiledOutput();

    std::unordered_set<std::string> fileNames;
    std::unordered_set<std::string> usedSections;
    std::vector<LinkerFile*> files;

    files.reserve(mProject->files.size());
    for (const auto& file : mProject->files) {
        if (!fileNames.emplace(file->name).second) {
            std::stringstream ss;
            ss << "duplicate file name \"" << file->name << "\".";
            throw CompilerError(file->nameLocation, ss.str());
        }
        files.emplace_back(new (mHeap) LinkerFile(usedSections, file.get(), mProgram));
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
                    throw CompilerError(section->location, ss.str());
                }
            }

            auto fileID = new (mHeap) FileID(mProject->path().filename(), mProject->path());
            auto location = new (mHeap) SourceLocation(fileID, -1);
            throw CompilerError(location, "unable to resolve sections.");
        }
    }

    for (auto& file : files) {
        file->generateCode(output->addFile(file->file()->location, file->file()->nameLocation,
            file->file()->name, file->takeDebugInfo()));
    }

    struct TestOnlySectionResolver : public ISectionResolver
    {
        const std::vector<LinkerFile*>& files;
        explicit TestOnlySectionResolver(const std::vector<LinkerFile*>& f) : files(f) {}

        bool tryResolveSectionAddress(const std::string& n, uint64_t&) const override { return isValidSectionName(n); }
        bool tryResolveSectionBase(const std::string& n, uint64_t&) const override { return isValidSectionName(n); }
        bool tryResolveSectionSize(const std::string& n, uint64_t&) const override { return isValidSectionName(n); }

        bool isValidSectionName(const std::string& name) const override
        {
            for (const auto& file : files) {
                if (file->isValidSectionName(name))
                    return true;
            }
            return false;
        }
    };

    for (const auto& it : mProgram->globals()->symbols()) {
        Symbol* symbol = it.second;
        switch (symbol->type()) {
            /* FIXME these checks cause errors for code not included in the build
            case Symbol::Label:
                if (!static_cast<LabelSymbol*>(symbol)->label()->hasAddress()) {
                    std::stringstream ss;
                    ss << "unable to resolve address for label \"" << symbol->name() << "\".";
                    throw CompilerError(symbol->location(), ss.str());
                }
                break;

            case Symbol::ConditionalLabel: {
                int64_t addr = 0;
                TestOnlySectionResolver testOnlyResolver(files);
                Label* label = static_cast<ConditionalLabelSymbol*>(symbol)->
                    label(symbol->location(), &addr, &testOnlyResolver);
                if (label && !label->hasAddress()) {
                    std::stringstream ss;
                    ss << "unable to resolve address for label \"" << symbol->name() << "\".";
                    throw CompilerError(symbol->location(), ss.str());
                }
                break;
            }
            */

            case Symbol::Constant: {
                int64_t addr = 0;
                TestOnlySectionResolver testOnlyResolver(files);
                static_cast<ConstantSymbol*>(symbol)->value()->evaluateValue(&addr, &testOnlyResolver);
                break;
            }

            case Symbol::ConditionalConstant: {
                int64_t addr = 0;
                TestOnlySectionResolver testOnlyResolver(files);
                Expr* expr = static_cast<ConditionalConstantSymbol*>(symbol)->
                    expr(symbol->location(), &addr, &testOnlyResolver);
                if (expr)
                    expr->evaluateValue(&addr, &testOnlyResolver);
                break;
            }
        }
    }

    return output;
}
