#include "Project.h"
#include "Compiler/Tree/SourceLocation.h"
#include "Compiler/Tree/Symbol.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/CompilerError.h"
#include "Compiler/ExpressionParser.h"
#include "Common/Xml.h"
#include "Common/IO.h"
#include <unordered_map>

const char* Project::FileSuffix = "retro";

Project::Project()
{
}

Project::~Project()
{
}

static void addConstant(SymbolTable* symbolTable, const Project::Constant& constant, SourceLocation* location)
{
    auto heap = symbolTable->heap();
    ExpressionParser parser(heap, nullptr, nullptr);
    Expr* value = parser.tryParseExpression(location, constant.value.c_str(), symbolTable);
    if (!value) {
        std::stringstream ss;
        ss << "unable to parse value for constant \"" << constant.name << "\": " << parser.error();
        throw CompilerError(parser.errorLocation(), ss.str());
    }

    auto symbol = new (heap) ConstantSymbol(location, constant.name.c_str(), value);
    if (!symbolTable->addSymbol(symbol)) {
        std::stringstream ss;
        ss << "duplicate constant \"" << constant.name << "\".";
        throw CompilerError(location, ss.str());
    }
}

void Project::setVariables(SymbolTable* symbolTable, const std::string& configuration) const
{
    auto heap = symbolTable->heap();
    auto fileID = new (heap) FileID(mPath.filename(), mPath);
    auto location = new (heap) SourceLocation(fileID, 1);

    for (const auto& constant : constants)
        addConstant(symbolTable, constant, location);

    for (const auto& it : configurations) {
        if (it->name == configuration) {
            for (const auto& constant : it->constants)
                addConstant(symbolTable, constant, location);
        }
    }
}

static std::unique_ptr<Project::Section> parseSection(const XmlDocument& xml, XmlNode xmlSection, Project::File* file)
{
    auto section = std::make_unique<Project::Section>();
    section->file = file;
    section->name = REQ_STRING(name, Section);
    section->base = OPT_STRING(base, Section);
    section->fileOffset = OPT_STRING(fileOffset, Section);
    section->alignment = OPT_STRING(alignment, Section);
    section->compression = Compression::None;

    auto comp = OPT_STRING(compression, Section);
    if (comp) {
        if (*comp == "zx7")
            section->compression = Compression::Zx7;
        else if (*comp == "lzsa2")
            section->compression = Compression::Lzsa2;
        else
            INVALID(compression, Section);
    }

    return section;
}

void Project::load(std::filesystem::path path)
{
    mPath = std::move(path);

    constants.clear();
    configurations.clear();
    files.clear();
    outputs.clear();

    auto xml = xmlLoad(mPath);
    ROOT(RetroProject);

    FOR_EACH(Constant, RetroProject) {
        Constant constant;
        constant.name = REQ_STRING(name, Constant);
        constant.value = REQ_STRING(value, Constant);
        constants.emplace_back(std::move(constant));
    }

    IF_HAS(Configuration, RetroProject) {
        auto config = std::make_unique<Configuration>();
        config->name = REQ_STRING(name, Configuration);

        FOR_EACH(Constant, Configuration) {
            Constant constant;
            constant.name = REQ_STRING(name, Constant);
            constant.value = REQ_STRING(value, Constant);
            config->constants.emplace_back(std::move(constant));
        }

        configurations.emplace_back(std::move(config));
    }

    IF_HAS(Files, RetroProject) {
        FOR_EACH(File, Files) {
            auto file = std::make_unique<File>();
            file->name = REQ_STRING(name, File);
            file->start = OPT_STRING(start, File);
            file->until = OPT_STRING(until, File);

            FOR_EACH(Section, Files)
                file->lowerSections.emplace_back(parseSection(xml, xmlSection, file.get()));

            FOR_EACH(UpperSection, Files)
                file->upperSections.emplace_back(parseSection(xml, xmlUpperSection, file.get()));

            files.emplace_back(std::move(file));
        }
    }

    IF_HAS(OutputTAP, RetroProject) {
        auto output = std::make_unique<Output>();
        output->type = Output::ZXSpectrumTAP;
        output->enabled = OPT_STRING(enabled, OutputTAP);

        FOR_EACH(File, OutputTAP) {
            Output::File outputFile = {};

            auto ref = OPT_STRING(ref, File);
            if (ref)
                outputFile.ref = std::move(ref);
            else {
                std::stringstream ss;
                ss << "Invalid <" << "File" << "> element in <"
                    << "OutputTAP" << "> section of file \"" << mPath.string() << "\".";
                throw std::runtime_error(ss.str());
            }

            output->files.emplace_back(std::move(outputFile));
        }

        outputs.emplace_back(std::move(output));
    }

    IF_HAS(OutputTRD, RetroProject) {
        auto output = std::make_unique<Output>();
        output->type = Output::ZXSpectrumTRD;
        output->enabled = OPT_STRING(enabled, OutputTRD);

        FOR_EACH(File, OutputTRD) {
            Output::File outputFile = {};

            auto ref = OPT_STRING(ref, File);
            if (ref)
                outputFile.ref = std::move(ref);
            else {
                std::stringstream ss;
                ss << "Invalid <" << "File" << "> element in <"
                    << "OutputTRD" << "> section of file \"" << mPath.string() << "\".";
                throw std::runtime_error(ss.str());
            }

            output->files.emplace_back(std::move(outputFile));
        }

        outputs.emplace_back(std::move(output));
    }
}

static void writeSection(std::stringstream& ss, const char* element, const Project::Section& section)
{
    ss << "            <Section name=";
    xmlEncodeInQuotes(ss, section.name);
    if (section.base) {
        ss << ' ';
        xmlEncodeInQuotes(ss, *section.base);
    }
    if (section.alignment) {
        ss << ' ';
        xmlEncodeInQuotes(ss, *section.alignment);
    }
    if (section.fileOffset) {
        ss << ' ';
        xmlEncodeInQuotes(ss, *section.fileOffset);
    }
    switch (section.compression) {
        case Compression::None: break;
        case Compression::Zx7: ss << "compression=\"" << "zx7" << '"'; break;
        case Compression::Lzsa2: ss << "compression=\"" << "lzsa2" << '"'; break;
    }
    ss << " />\n";
}

static void writeOutput(std::stringstream& ss, const Project::Output& output)
{
    const char* element = nullptr;
    switch (output.type) {
        case Project::Output::ZXSpectrumTAP: element = "OutputTAP"; break;
        case Project::Output::ZXSpectrumTRD: element = "OutputTRD"; break;
    }

    ss << "    <" << element;
    if (output.enabled) {
        ss << " enabled=";
        xmlEncodeInQuotes(ss, *output.enabled);
    }
    ss << ">\n";

    for (const auto& file : output.files) {
        ss << "        <File";
        if (file.ref) {
            ss << " ref=";
            xmlEncodeInQuotes(ss, *file.ref);
        }
        ss << " />\n";
    }

    ss << "    </" << element << ">\n";
}

void Project::save(std::filesystem::path path, bool createNew)
{
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    ss << "<RetroProject>\n";
    if (!constants.empty()) {
        ss << '\n';
        for (const auto& it : constants) {
            ss << "    <Constant name=";
            xmlEncodeInQuotes(ss, it.name);
            ss << " value=";
            xmlEncodeInQuotes(ss, it.value);
            ss << " />\n";
        }
    }
    if (!configurations.empty()) {
        ss << '\n';
        for (const auto& config : configurations) {
            ss << "    <Configuration name=";
            xmlEncodeInQuotes(ss, config->name);
            ss << ">\n";
            for (const auto& it : config->constants) {
                ss << "        <Constant name=";
                xmlEncodeInQuotes(ss, it.name);
                ss << " value=";
                xmlEncodeInQuotes(ss, it.value);
                ss << " />\n";
            }
            ss << "    </Configuration>\n";
        }
    }
    if (!files.empty()) {
        ss << '\n';
        ss << "    <Files>\n";
        for (const auto& file : files) {
            ss << "        <File name=";
            xmlEncodeInQuotes(ss, file->name);
            if (file->start) {
                ss << ' ';
                xmlEncodeInQuotes(ss, *file->start);
            }
            if (file->until) {
                ss << ' ';
                xmlEncodeInQuotes(ss, *file->until);
            }
            ss << ">\n";
            for (const auto& section : file->lowerSections)
                writeSection(ss, "Section", *section);
            for (const auto& section : file->upperSections)
                writeSection(ss, "SectionUpper", *section);
            ss << "        </File>\n";
        }
        ss << "    </Files>\n";
    }
    if (!outputs.empty()) {
        for (const auto& output : outputs) {
            ss << '\n';
            writeOutput(ss, *output);
        }
    }
    ss << '\n';
    ss << "</RetroProject>\n";

    writeFile(path, ss.str());
    mPath = std::move(path);

    if (createNew) {
        /* FIXME */
    }
}
