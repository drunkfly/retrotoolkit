#ifndef COMPILER_BASICCOMPILER_H
#define COMPILER_BASICCOMPILER_H

#include "Common/Common.h"
#include <map>
#include <sstream>
#include <string>

class GCHeap;
class CompiledOutput;
class SourceFile;

class BasicCompiler
{
public:
    BasicCompiler(GCHeap* heap, CompiledOutput* output);
    ~BasicCompiler();

    void addFile(SourceFile* source);
    void compile();

    std::string compiledData() const { return mCompiledBasicStream.str(); }

private:
    struct Line
    {
        SourceFile* file;
        int line;
        int basicIndex;
        std::string lineData;
    };

    GCHeap* mHeap;
    CompiledOutput* mOutput;
    std::map<int, Line> mLines;
    std::map<int, Line>::const_iterator mLinesIter;
    std::stringstream mCompiledBasicStream;
    SourceFile* mBasicFile;
    int mBasicFileLine;

    static void bas2tapError(int line, int stmt, const char* fmt, ...);
    static int bas2tapFGets(char** basicIndex, int* basicLineNo);
    static void bas2tapOutput(const void* dst, size_t length);

    DISABLE_COPY(BasicCompiler);
};

#endif
