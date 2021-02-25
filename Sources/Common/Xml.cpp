#include "Xml.h"
#include "Common/Strings.h"
#include "Common/IO.h"
#include <stdexcept>
#include <sstream>
#include <string.h>

XmlDocument xmlLoad(const std::filesystem::path& path)
{
    auto xml = std::make_unique<RapidXmlDocument>();
    xml->data = loadFile(path);
    xml->path = path;
    xml->doc.parse<0>(&xml->data[0]);

    return xml;
}

XmlNode xmlGetRootElement(const XmlDocument& xml, const char* name)
{
    auto xmlRoot = xml->doc.first_node();
    if (strcmp(xmlRoot->name(), name) != 0) {
        std::stringstream ss;
        ss << "File \"" << xml->path.string() << "\" has invalid root element.";
        throw std::runtime_error(ss.str());
    }
    return xmlRoot;
}

std::string xmlGetRequiredAttribute(const XmlDocument& xml, XmlNode node, const char* name)
{
    auto attr = node->first_attribute(name);
    if (!attr)
        xmlMissingAttributeValue(xml, node, name);

    return std::string(attr->value(), attr->value_size());
}

std::optional<std::string> xmlGetOptionalAttribute(const XmlDocument& xml, XmlNode node, const char* name)
{
    std::optional<std::string> result;

    auto attr = node->first_attribute(name);
    if (attr)
        result = std::string(attr->value(), attr->value_size());

    return result;
}

int xmlGetRequiredIntAttribute(const XmlDocument& xml, XmlNode node, const char* name)
{
    auto attr = node->first_attribute(name);
    if (!attr)
        xmlMissingAttributeValue(xml, node, name);

    int result;
    if (!intFromString(result, attr->value(), attr->value_size()))
        xmlInvalidAttributeValue(xml, node, name);

    return result;
}

std::optional<int> xmlGetOptionalIntAttribute(const XmlDocument& xml, XmlNode node, const char* name)
{
    std::optional<int> result;

    auto attr = node->first_attribute(name);
    if (!attr)
        return result;

    int v;
    if (!intFromString(v, attr->value(), attr->value_size()))
        xmlInvalidAttributeValue(xml, node, name);

    result = v;
    return result;
}

bool xmlGetRequiredBoolAttribute(const XmlDocument& xml, XmlNode node, const char* name)
{
    auto attr = node->first_attribute(name);
    if (!attr)
        xmlMissingAttributeValue(xml, node, name);

    bool result;
    if (!boolFromString(result, attr->value(), attr->value_size()))
        xmlInvalidAttributeValue(xml, node, name);

    return result;
}

std::optional<bool> xmlGetOptionalBoolAttribute(const XmlDocument& xml, XmlNode node, const char* name)
{
    std::optional<bool> result;

    auto attr = node->first_attribute(name);
    if (!attr)
        return result;

    bool b;
    if (!boolFromString(b, attr->value(), attr->value_size()))
        xmlInvalidAttributeValue(xml, node, name);

    result = b;
    return result;
}

[[noreturn]] void xmlMissingAttributeValue(const XmlDocument& xml, XmlNode node, const char* name)
{
    std::stringstream ss;
    ss << "Missing required attribute \"" << name << "\" in element \""
        << node->value() << "\" in file \"" << xml->path.string() << "\".";
    throw std::runtime_error(ss.str());
}

[[noreturn]] void xmlInvalidAttributeValue(const XmlDocument& xml, XmlNode node, const char* name)
{
    std::stringstream ss;
    ss << "Invalid value \"" << node->first_attribute(name)->value() << "\" for attribute \"" << name
        << "\" in element \"" << node->value() << "\" in file \"" << xml->path.string() << "\".";
    throw std::runtime_error(ss.str());
}

void xmlEncode(std::stringstream& ss, const std::string& str)
{
    for (char ch : str) {
        switch (ch) {
            case '<': ss << "&lt;"; break;
            case '>': ss << "&gt;"; break;
            case '&': ss << "&amp;"; break;
            case '"': ss << "&quot;"; break;
            default: ss << ch; break;
        }
    }
}

void xmlEncodeInQuotes(std::stringstream& ss, const std::string& str)
{
    ss << '"';
    xmlEncode(ss, str);
    ss << '"';
}
