#include "Project.h"
#include "Common/IO.h"
#include <sstream>
#include <rapidxml.hpp>

const char* Project::FileSuffix = "retro";

static const char XmlRootElement[] = "RetroProject";

Project::Project()
{
}

Project::~Project()
{
}

void Project::load(const std::string& file)
{
    std::string text = loadFile(file);

    rapidxml::xml_document<> xml;
    xml.parse<0>(&text[0]);

    auto node = xml.first_node();
    if (strcmp(node->name(), XmlRootElement) != 0) {
        std::stringstream ss;
        ss << "File \"" << file << "\" has invalid root element.";
        throw std::runtime_error(ss.str());
    }
}

void Project::save(const std::string& file, bool createNew)
{
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    ss << "<" << XmlRootElement << ">\n";
    ss << "</" << XmlRootElement << ">\n";
    writeFile(file, ss.str());

    if (createNew) {
        /* FIXME */
    }
}
