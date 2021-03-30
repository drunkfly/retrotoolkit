#ifndef COMPILER_SPECTRUMBASICCOMPILER_H
#define COMPILER_SPECTRUMBASICCOMPILER_H

#include "Common/Common.h"

class GCHeap;
class CompiledOutput;
class SourceLocation;
struct SourceFile;

class SpectrumBasicCompiler
{
public:
    SpectrumBasicCompiler(GCHeap* heap, CompiledOutput* output);
    ~SpectrumBasicCompiler();

    bool hasStartLine() const { return mStartLine.has_value(); }
    int startLine() const { return mStartLine.value(); }

    void addFile(const SourceFile* source);
    void compile();

    std::string compiledData() const { return mCompiledBasicStream.str(); }

private:
    struct Line
    {
        const SourceFile* file;
        int line;
        int basicIndex;
        std::string lineData;
    };

    GCHeap* mHeap;
    CompiledOutput* mOutput;
    std::map<int, Line> mLines;
    std::map<int, Line>::const_iterator mLinesIter;
    std::stringstream mCompiledBasicStream;
    const SourceFile* mBasicFile;
    SourceLocation* mSourceLocation;
    SourceLocation* mStartLineLocation;
    std::optional<int> mStartLine;
    int mBasicFileLine;

    static void bas2tapError(int line, int stmt, const char* fmt, ...);
    static int bas2tapFGets(char** basicIndex, int* basicLineNo);
    static void bas2tapOutput(const void* dst, size_t length);

    DISABLE_COPY(SpectrumBasicCompiler);
};

#endif
