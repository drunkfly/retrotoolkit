#include "SpectrumBasicCompiler.h"
#include "Compiler/SourceFile.h"
#include "Compiler/CompilerError.h"
#include "Compiler/LexerUtils.h"
#include "Linker/CompiledOutput.h"
#include "Common/IO.h"
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <bas2tap.h>

#define APPEND(C) \
    if (d >= MAXLINELENGTH) \
        throw CompilerError(new (mHeap) SourceLocation(source->fileID, line), "line is too long."); \
    else \
        lineIn[d++] = (C)

#define APPENDSTR(STR) \
    for (const char* str = (STR); *str; ++str) \
        APPEND(*str)

static SpectrumBasicCompiler* instance;

SpectrumBasicCompiler::SpectrumBasicCompiler(GCHeap* heap, CompiledOutput* output)
    : mHeap(heap)
    , mOutput(output)
    , mBasicFile(nullptr)
    , mSourceLocation(nullptr)
    , mStartLineLocation(nullptr)
    , mBasicFileLine(0)
{
    assert(instance == nullptr);
    instance = this;

    bas2tap_error = bas2tapError;
    bas2tap_fgets = bas2tapFGets;
    bas2tap_output = bas2tapOutput;

    bas2tap_reset();
}

SpectrumBasicCompiler::~SpectrumBasicCompiler()
{
    assert(instance == this);
    instance = nullptr;
}

void SpectrumBasicCompiler::addFile(const SourceFile* source)
{
    if (!mSourceLocation)
        mSourceLocation = new (mHeap) SourceLocation(source->fileID, 1);

    std::string fileData = loadFile(source->fileID->path());

    const char* p = fileData.c_str();
    const char* end = p + fileData.length();
    int line = 0;
    while (p < end) {
        ++line;
        mBasicFile = source;
        mBasicFileLine = line;

        if (!*p) {
            throw CompilerError(
                new (mHeap) SourceLocation(source->fileID, line), "unexpected NUL byte in source file.");
        }

        char lineIn[MAXLINELENGTH + 1];
        int d = 0;
        while (*p) {
            char ch = *p++;

            if (ch == '/' && *p == '/') {
                ++p;
                while (*p && *p != '\n')
                    ++p;
                if (*p == '\n')
                    ++p;
                APPEND('\n');
                break;
            }

            if (ch == '\r' && *p == '\n') {
                ++p;
                ch = '\n';
            }

            if (ch == '@' && *p == '{') {
                const char* pend = p + 1;
                while (*pend && *pend != '}')
                    ++pend;
                if (*pend == '}') {
                    int n = int(pend - (p + 1));
                    if (n > 5 && !strncmp(p + 1, "file:", 5)) {
                        std::string name(p + 6, n - 5);
                        auto file = mOutput->getFile(name);
                        if (!file) {
                            std::stringstream ss;
                            ss << "there is no file named \"" << name << "\".";
                            throw CompilerError(new (mHeap) SourceLocation(source->fileID, line), ss.str());
                        }
                        file->setUsedByBasic();
                        size_t n = file->size();
                        const CodeEmitter::Byte* fileP = file->data();
                        for (size_t i = 0; i < n; ++i, ++fileP) {
                            char buf[8];
                            snprintf(buf, sizeof(buf), "{%02X}", fileP->value);
                            APPENDSTR(buf);
                        }
                        p = pend + 1;
                        continue;
                    } else if (n > 10 && !strncmp(p + 1, "autostart:", 10)) {
                        mStartLineLocation = new (mHeap) SourceLocation(source->fileID, line);
                        p += 11;

                        if (p == pend)
                            throw CompilerError(mStartLineLocation, "missing line number.");

                        uint64_t number = 0;
                        while (p < pend) {
                            if (!isDigit(*p))
                                throw CompilerError(mStartLineLocation, "syntax error in number.");
                            number *= 10;
                            number += charToInt(*p++);
                            if (number >= 10000)
                                break;
                        }

                        if (number == 0 || number >= 10000)
                            throw CompilerError(mStartLineLocation, "autostart line number is out of range.");

                        if (mStartLine && mStartLine.value() != number) {
                            throw CompilerError(mStartLineLocation,
                                "conflicting line numbers in \"autostart\" directives.");
                        }

                        mStartLine = int(number);
                        p = pend + 1;

                        continue;
                    }

                    std::stringstream ss;
                    ss << "invalid directive \"";
                    ss.write(p + 1, n);
                    ss << "\".";
                    throw CompilerError(new (mHeap) SourceLocation(source->fileID, line), ss.str());
                }
            }

            APPEND(ch);

            if (ch == '\n')
                break;
        }
        lineIn[d] = 0;

        char* basicIndex = nullptr;
        int basicLine = bas2tap_PrepareLine(lineIn, line, &basicIndex);
        if (basicLine < 0) {
            if (basicLine == -1)
                throw CompilerError(new (mHeap) SourceLocation(source->fileID, line), "compilation error");
            continue; // line should be skipped
        }

        Line l;
        l.file = source;
        l.line = line;
        l.basicIndex = int(basicIndex - bas2tap_ConvertedSpectrumLine);
        l.lineData = bas2tap_ConvertedSpectrumLine;
        if (!mLines.emplace(basicLine, std::move(l)).second) {
            std::stringstream ss;
            ss << "duplicate use of line number " << basicLine << '.';
            throw CompilerError(new (mHeap) SourceLocation(source->fileID, line), ss.str());
        }
    }
}

void SpectrumBasicCompiler::compile()
{
    mLinesIter = mLines.cbegin();
    mBasicFile = nullptr;
    mBasicFileLine = 0;
    if (bas2tap_main() != 0)
        throw CompilerError(mSourceLocation, "BASIC compilation failed.");

    if (mStartLine) {
        auto it = mLines.find(*mStartLine);
        if (it == mLines.end()) {
            std::stringstream ss;
            ss << "line " << *mStartLine << " does not exist.";
            throw CompilerError(mStartLineLocation, ss.str());
        }
    }
}

void SpectrumBasicCompiler::bas2tapError(int line, int stmt, const char* fmt, ...)
{
    char buf[1024];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    auto file = (instance->mBasicFile ? instance->mBasicFile->fileID : nullptr);
    throw CompilerError(new (instance->mHeap) SourceLocation(file, instance->mBasicFileLine), buf);
}

int SpectrumBasicCompiler::bas2tapFGets(char** basicIndex, int* basicLineNo)
{
    if (instance->mLinesIter == instance->mLines.end())
        return 0;

    int basicLine = instance->mLinesIter->first;
    const Line& line = instance->mLinesIter->second;
    ++instance->mLinesIter;

    instance->mBasicFile = line.file;
    instance->mBasicFileLine = line.line;
    memcpy(bas2tap_ConvertedSpectrumLine, line.lineData.c_str(), line.lineData.length() + 1);
    *basicIndex = bas2tap_ConvertedSpectrumLine + line.basicIndex;
    *basicLineNo = basicLine;

    return 1;
}

void SpectrumBasicCompiler::bas2tapOutput(const void* dst, size_t length)
{
    instance->mCompiledBasicStream.write(reinterpret_cast<const char*>(dst), int(length));
}
