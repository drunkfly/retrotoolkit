#ifndef COMPILER_OUTPUT_IOUTPUTWRITERPROXY_H
#define COMPILER_OUTPUT_IOUTPUTWRITERPROXY_H

#include "Compiler/Output/IOutputWriter.h"
#include "Compiler/Project.h"

class IOutputWriterProxy : public IOutputWriter
{
public:
    virtual void setOutput(Project::Output::Type type, IOutputWriter* writer) = 0;
};

#endif
