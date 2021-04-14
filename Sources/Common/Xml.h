#ifndef COMMON_XML_H
#define COMMON_XML_H

#include "Common/Common.h"
#include <tinyxml.h>

#define ROOT(NAME) \
    XmlNode xml##NAME = xmlGetRootElement(xml, #NAME)

#define IF_HAS(NAME, PARENT) \
    if (auto xml##NAME = xml##PARENT->FirstChildElement(#NAME); (xml##NAME ? xml##NAME->accessed = true : false))
#define FOR_EACH(NAME, PARENT) \
    for (auto xml##NAME = xml##PARENT->FirstChildElement(#NAME); (xml##NAME ? xml##NAME->accessed = true : false); \
        xml##NAME = xml##NAME->NextSiblingElement(#NAME))

#define ROW(NAME) \
    xml##NAME->Row()
#define ATTR_ROW(NAME, PARENT) \
    xmlGetAttributeRow(xml##PARENT, #NAME)

#define REQ_STRING(NAME, PARENT) \
    xmlGetRequiredAttribute(xml, xml##PARENT, #NAME)
#define OPT_STRING(NAME, PARENT) \
    xmlGetOptionalAttribute(xml, xml##PARENT, #NAME)

#define REQ_INT(NAME, PARENT) \
    xmlGetRequiredIntAttribute(xml, xml##PARENT, #NAME)
#define OPT_INT(NAME, PARENT) \
    xmlGetOptionalIntAttribute(xml, xml##PARENT, #NAME)

#define REQ_BOOL(NAME, PARENT) \
    xmlGetRequiredBoolAttribute(xml, xml##PARENT, #NAME)
#define OPT_BOOL(NAME, PARENT) \
    xmlGetOptionalBoolAttribute(xml, xml##PARENT, #NAME)

#define INVALID(NAME, PARENT) \
    xmlInvalidAttributeValue(xml, xml##PARENT, #NAME)

struct TinyXmlDocument
{
    std::filesystem::path path;
    std::string data;
    TiXmlDocument doc;
};

using XmlDocument = std::unique_ptr<TinyXmlDocument>;
using XmlNode = const TiXmlElement*;

XmlDocument xmlLoad(const std::filesystem::path& path);
XmlNode xmlGetRootElement(const XmlDocument& xml, const char* name);

const std::string& xmlGetRequiredAttribute(const XmlDocument& xml, XmlNode node, const char* name);
std::optional<std::string> xmlGetOptionalAttribute(const XmlDocument& xml, XmlNode node, const char* name);
int xmlGetRequiredIntAttribute(const XmlDocument& xml, XmlNode node, const char* name);
std::optional<int> xmlGetOptionalIntAttribute(const XmlDocument& xml, XmlNode node, const char* name);
bool xmlGetRequiredBoolAttribute(const XmlDocument& xml, XmlNode node, const char* name);
std::optional<bool> xmlGetOptionalBoolAttribute(const XmlDocument& xml, XmlNode node, const char* name);

int xmlGetAttributeRow(XmlNode node, const char* name);

[[noreturn]] void xmlMissingAttributeValue(const XmlDocument& xml, XmlNode node, const char* name);
[[noreturn]] void xmlInvalidAttributeValue(const XmlDocument& xml, XmlNode node, const char* name);

void xmlEncode(std::stringstream& ss, const std::string& str);
void xmlEncodeInQuotes(std::stringstream& ss, const std::string& str);

void xmlCheckAllAccessed(const XmlDocument& xml);

#endif
