#ifndef COMPILER_LINKER_ISECTIONRESOLVER_H
#define COMPILER_LINKER_ISECTIONRESOLVER_H

#include "Common/Common.h"

class SourceLocation;

class ISectionResolver
{
public:
    virtual ~ISectionResolver() = default;
    virtual bool isValidSectionName(SourceLocation* location, const std::string& name) const = 0;
    virtual bool tryResolveSectionAddress(SourceLocation* location, const std::string& name, uint64_t& value) const = 0;
    virtual bool tryResolveSectionBase(SourceLocation* location, const std::string& name, uint64_t& value) const = 0;
    virtual bool tryResolveSectionSize(SourceLocation* location, const std::string& name, uint64_t& value) const = 0;
};

#endif
