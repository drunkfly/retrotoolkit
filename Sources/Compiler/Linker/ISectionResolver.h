#ifndef COMPILER_LINKER_ISECTIONRESOLVER_H
#define COMPILER_LINKER_ISECTIONRESOLVER_H

#include "Common/Common.h"

class ISectionResolver
{
public:
    virtual ~ISectionResolver() = default;
    virtual bool isValidSectionName(const std::string& sectionName) const = 0;
    virtual bool tryResolveSectionAddress(const std::string& sectionName, uint64_t& value) const = 0;
    virtual bool tryResolveSectionBase(const std::string& sectionName, uint64_t& value) const = 0;
    virtual bool tryResolveSectionSize(const std::string& sectionName, uint64_t& value) const = 0;
};

#endif
