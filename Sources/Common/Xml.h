#ifndef COMMON_XML_H
#define COMMON_XML_H

#include <rapidxml.hpp>
#include <memory>
#include <optional>
#include <string>
#include <sstream>
#include <filesystem>

#define ROOT(NAME) \
    XmlNode xml##NAME = xmlGetRootElement(xml, #NAME)
#define IF_HAS(NAME, PARENT) \
    if (auto xml##NAME = xml##PARENT->first_node(#NAME); xml##NAME)
#define FOR_EACH(NAME, PARENT) \
    for (auto xml##NAME = xml##PARENT->first_node(#NAME); xml##NAME; xml##NAME = xml##NAME->next_sibling(#NAME))

#define REQ_STRING(NAME, PARENT) \
    xmlGetRequiredAttribute(xml, xml##PARENT, #NAME)
#define OPT_STRING(NAME, PARENT) \
    xmlGetOptionalAttribute(xml, xml##PARENT, #NAME)

#define REQ_BOOL(NAME, PARENT) \
    xmlGetRequiredBoolAttribute(xml, xml##PARENT, #NAME)
#define OPT_BOOL(NAME, PARENT) \
    xmlGetOptionalBoolAttribute(xml, xml##PARENT, #NAME)

#define INVALID(NAME, PARENT) \
    xmlInvalidAttributeValue(xml, xml##PARENT, #NAME)

struct RapidXmlDocument
{
    std::filesystem::path path;
    std::string data;
    rapidxml::xml_document<> doc;
};

using XmlDocument = std::unique_ptr<RapidXmlDocument>;
using XmlNode = rapidxml::xml_node<>*;

XmlDocument xmlLoad(const std::filesystem::path& path);
XmlNode xmlGetRootElement(const XmlDocument& xml, const char* name);

std::string xmlGetRequiredAttribute(const XmlDocument& xml, XmlNode node, const char* name);
std::optional<std::string> xmlGetOptionalAttribute(const XmlDocument& xml, XmlNode node, const char* name);
bool xmlGetRequiredBoolAttribute(const XmlDocument& xml, XmlNode node, const char* name);
std::optional<bool> xmlGetOptionalBoolAttribute(const XmlDocument& xml, XmlNode node, const char* name);

[[noreturn]] void xmlMissingAttributeValue(const XmlDocument& xml, XmlNode node, const char* name);
[[noreturn]] void xmlInvalidAttributeValue(const XmlDocument& xml, XmlNode node, const char* name);

void xmlEncode(std::stringstream& ss, const std::string& str);
void xmlEncodeInQuotes(std::stringstream& ss, const std::string& str);

#endif
