#include "Xml.h"
#include "Common/Strings.h"
#include "Common/IO.h"

XmlDocument xmlLoad(const std::filesystem::path& path)
{
    auto xml = std::make_unique<TinyXmlDocument>();

    xml->path = path;
    xml->data = loadFile(path);
    if (xml->data.empty()) {
        std::stringstream ss;
        ss << "File \"" << xml->path.string() << "\" is empty.";
        throw std::runtime_error(ss.str());
    }

    if (!xml->doc.LoadMemory(&xml->data[0], xml->data.length(), TIXML_ENCODING_UTF8)) {
        std::stringstream ss;
        ss << "Parse error in XML file \"" << xml->path.string() << "\" at line "
            << xml->doc.ErrorRow() << ", column " << xml->doc.ErrorCol() << ": " << xml->doc.ErrorDesc();
        throw std::runtime_error(ss.str());
    }

    return xml;
}

XmlNode xmlGetRootElement(const XmlDocument& xml, const char* name)
{
    const TiXmlElement* xmlRoot = xml->doc.RootElement();
    if (!xmlRoot || strcmp(xmlRoot->Value(), name) != 0) {
        std::stringstream ss;
        ss << "File \"" << xml->path.string() << "\" has invalid root element.";
        throw std::runtime_error(ss.str());
    }
    xmlRoot->accessed = true;
    return xmlRoot;
}

const std::string& xmlGetRequiredAttribute(const XmlDocument& xml, XmlNode node, const char* name)
{
    size_t len = strlen(name);
    for (const TiXmlAttribute* attr = node->FirstAttribute(); attr; attr = attr->Next()) {
        if (attr->NameStr().length() == len && !strcmp(attr->Name(), name)) {
            attr->accessed = true;
            return attr->ValueStr();
        }
    }
    xmlMissingAttributeValue(xml, node, name);
}

std::optional<std::string> xmlGetOptionalAttribute(const XmlDocument& xml, XmlNode node, const char* name)
{
    std::optional<std::string> result;

    size_t len = strlen(name);
    for (const TiXmlAttribute* attr = node->FirstAttribute(); attr; attr = attr->Next()) {
        if (attr->NameStr().length() == len && !strcmp(attr->Name(), name)) {
            attr->accessed = true;
            result = attr->ValueStr();
            break;
        }
    }

    return result;
}

int xmlGetRequiredIntAttribute(const XmlDocument& xml, XmlNode node, const char* name)
{
    const std::string& str = xmlGetRequiredAttribute(xml, node, name);

    int result;
    if (!intFromString(result, str.c_str(), str.length()))
        xmlInvalidAttributeValue(xml, node, name);

    return result;
}

std::optional<int> xmlGetOptionalIntAttribute(const XmlDocument& xml, XmlNode node, const char* name)
{
    std::optional<int> result;

    size_t len = strlen(name);
    for (const TiXmlAttribute* attr = node->FirstAttribute(); attr; attr = attr->Next()) {
        if (attr->NameStr().length() == len && !strcmp(attr->Name(), name)) {
            int value;
            if (!intFromString(value, attr->Value(), attr->ValueStr().length()))
                xmlInvalidAttributeValue(xml, node, name);
            result = value;
            attr->accessed = true;
            break;
        }
    }

    return result;
}

bool xmlGetRequiredBoolAttribute(const XmlDocument& xml, XmlNode node, const char* name)
{
    const std::string& str = xmlGetRequiredAttribute(xml, node, name);

    bool result;
    if (!boolFromString(result, str.c_str(), str.length()))
        xmlInvalidAttributeValue(xml, node, name);

    return result;
}

std::optional<bool> xmlGetOptionalBoolAttribute(const XmlDocument& xml, XmlNode node, const char* name)
{
    std::optional<bool> result;

    size_t len = strlen(name);
    for (const TiXmlAttribute* attr = node->FirstAttribute(); attr; attr = attr->Next()) {
        if (attr->NameStr().length() == len && !strcmp(attr->Name(), name)) {
            bool value;
            if (!boolFromString(value, attr->Value(), attr->ValueStr().length()))
                xmlInvalidAttributeValue(xml, node, name);
            result = value;
            attr->accessed = true;
            break;
        }
    }

    return result;
}

int xmlGetAttributeRow(XmlNode node, const char* name)
{
    size_t len = strlen(name);
    for (const TiXmlAttribute* attr = node->FirstAttribute(); attr; attr = attr->Next()) {
        if (attr->NameStr().length() == len && !strcmp(attr->Name(), name))
            return attr->Row();
    }

    return node->Row();
}

[[noreturn]] void xmlMissingElement(const XmlDocument& xml, XmlNode node, const char* name)
{
    std::stringstream ss;
    ss << "Missing required element \"" << name << "\" in element \""
        << node->ValueStr() << "\" in file \"" << xml->path.string() << "\" at line "
        << node->Row() << ", column " << node->Column() << '.';
    throw std::runtime_error(ss.str());
}

[[noreturn]] void xmlMissingAttributeValue(const XmlDocument& xml, XmlNode node, const char* name)
{
    std::stringstream ss;
    ss << "Missing required attribute \"" << name << "\" in element \""
        << node->ValueStr() << "\" in file \"" << xml->path.string() << "\" at line "
        << node->Row() << ", column " << node->Column() << '.';
    throw std::runtime_error(ss.str());
}

[[noreturn]] void xmlInvalidAttributeValue(const XmlDocument& xml, XmlNode node, const char* name)
{
    std::stringstream ss;
    ss << "Invalid value \"" << node->Attribute(name) << "\" for attribute \"" << name
        << "\" in element \"" << node->ValueStr() << "\" in file \"" << xml->path.string() << "\" at line "
        << node->Row() << ", column " << node->Column() << '.';
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

static void xmlCheckAllAccessed(const XmlDocument& xml, const TiXmlElement* element)
{
    if (!element->accessed) {
        std::stringstream ss;
        ss << "Unknown element \"" << element->ValueStr() << "\" in file \"" << xml->path.string() << "\" at line "
           << element->Row() << ", column " << element->Column() << '.';
        throw std::runtime_error(ss.str());
    }

    for (const TiXmlAttribute* attr = element->FirstAttribute(); attr; attr = attr->Next()) {
        if (!attr->accessed) {
            std::stringstream ss;
            ss << "Unknown attribute \"" << attr->Name() << "\" in element \"" << element->ValueStr()
               << "\" in file \"" << xml->path.string() << "\" at line " << attr->Row()
               << ", column " << attr->Column() << '.';
            throw std::runtime_error(ss.str());
        }
    }

    for (const TiXmlElement* child = element->FirstChildElement(); child; child = child->NextSiblingElement())
        xmlCheckAllAccessed(xml, child);
}

void xmlCheckAllAccessed(const XmlDocument& xml)
{
    const TiXmlElement* xmlRoot = xml->doc.RootElement();
    if (!xmlRoot)
        return;

    xmlCheckAllAccessed(xml, xmlRoot);
}
