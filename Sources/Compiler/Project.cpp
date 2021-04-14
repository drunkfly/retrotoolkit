#include "Project.h"
#include "Compiler/Tree/Expr.h"
#include "Compiler/Tree/SourceLocation.h"
#include "Compiler/Tree/SourceLocationFactory.h"
#include "Compiler/Tree/Symbol.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/CompilerError.h"
#include "Compiler/ExpressionParser.h"
#include "Common/Xml.h"
#include "Common/IO.h"

const char* Project::FileSuffix = "retro";
const char* Project::DefaultOutputDirectory = "_out";

Project::Project()
{
    outputDirectory = DefaultOutputDirectory;
}

Project::~Project()
{
}

static void addConstant(SymbolTable* symbolTable, const Project::Constant& constant, SourceLocation* location)
{
    auto heap = symbolTable->heap();
    ExpressionParser parser(heap, nullptr, nullptr, nullptr);
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

    for (const auto& constant : constants)
        addConstant(symbolTable, constant, constant.location);

    for (const auto& it : configurations) {
        if (it->name == configuration) {
            for (const auto& constant : it->constants)
                addConstant(symbolTable, constant, constant.location);
        }
    }
}

static std::unique_ptr<Project::Section> parseSection(
    const XmlDocument& xml, XmlNode xmlSection, Project::File* file, SourceLocationFactory* locFactory)
{
    auto section = std::make_unique<Project::Section>();
    section->file = file;
    section->location = (locFactory ? locFactory->createLocation(ROW(Section)) : nullptr);
    section->name = REQ_STRING(name, Section);
    section->nameLocation = (locFactory ? locFactory->createLocation(ATTR_ROW(name, Section)) : nullptr);
    section->base = OPT_STRING(base, Section);
    section->baseLocation = (locFactory ? locFactory->createLocation(ATTR_ROW(base, Section)) : nullptr);
    section->fileOffset = OPT_STRING(fileOffset, Section);
    section->fileOffsetLocation = (locFactory ? locFactory->createLocation(ATTR_ROW(fileOffset, Section)) : nullptr);
    section->alignment = OPT_STRING(alignment, Section);
    section->alignmentLocation = (locFactory ? locFactory->createLocation(ATTR_ROW(alignment, Section)) : nullptr);
    section->attachment = Project::Section::Attachment::Default;
    section->compression = Compression::None;
    section->compressionLocation = section->location;

    auto attach = OPT_STRING(attachment, Section);
    if (attach) {
        if (*attach == "lower")
            section->attachment = Project::Section::Attachment::Lower;
        else if (*attach == "upper")
            section->attachment = Project::Section::Attachment::Upper;
        else
            INVALID(attachment, Section);
    }

    auto comp = OPT_STRING(compression, Section);
    if (comp) {
        if (locFactory)
            section->compressionLocation = locFactory->createLocation(ATTR_ROW(compression, Section));

        if (*comp == "zx7")
            section->compression = Compression::Zx7;
        else if (*comp == "zx0")
            section->compression = Compression::Zx0;
        else if (*comp == "zx0-quick")
            section->compression = Compression::Zx0Quick;
        else if (*comp == "lzsa2")
            section->compression = Compression::Lzsa2;
        else
            INVALID(compression, Section);
    }

    return section;
}

void Project::load(std::filesystem::path path, SourceLocationFactory* locationFactory)
{
    mPath = std::move(path);

    constants.clear();
    configurations.clear();
    files.clear();
    outputs.clear();
    outputDirectory.reset();

    auto xml = xmlLoad(mPath);
    ROOT(RetroProject);

    if (locationFactory)
        locationFactory->setFileName(mPath.filename(), mPath);

    IF_HAS(OutputDirectory, RetroProject) {
        outputDirectory = OPT_STRING(path, OutputDirectory);
    }

    FOR_EACH(Constant, RetroProject) {
        Constant constant;
        constant.location = (locationFactory ? locationFactory->createLocation(ROW(Constant)) : nullptr);
        constant.name = REQ_STRING(name, Constant);
        constant.value = REQ_STRING(value, Constant);
        constants.emplace_back(std::move(constant));
    }

    FOR_EACH(Configuration, RetroProject) {
        auto config = std::make_unique<Configuration>();
        config->name = REQ_STRING(name, Configuration);

        FOR_EACH(Constant, Configuration) {
            Constant constant;
            constant.location = (locationFactory ? locationFactory->createLocation(ROW(Constant)) : nullptr);
            constant.name = REQ_STRING(name, Constant);
            constant.value = REQ_STRING(value, Constant);
            config->constants.emplace_back(std::move(constant));
        }

        configurations.emplace_back(std::move(config));
    }

    IF_HAS(Files, RetroProject) {
        FOR_EACH(File, Files) {
            auto file = std::make_unique<File>();
            file->location = (locationFactory ? locationFactory->createLocation(ROW(File)) : nullptr);
            file->name = REQ_STRING(name, File);
            file->nameLocation = (locationFactory ? locationFactory->createLocation(ATTR_ROW(name, File)) : nullptr);
            file->start = OPT_STRING(start, File);
            file->startLocation = (locationFactory ? locationFactory->createLocation(ATTR_ROW(start, File)) : nullptr);
            file->until = OPT_STRING(until, File);
            file->untilLocation = (locationFactory ? locationFactory->createLocation(ATTR_ROW(until, File)) : nullptr);

            FOR_EACH(Section, File)
                file->sections.emplace_back(parseSection(xml, xmlSection, file.get(), locationFactory));

            files.emplace_back(std::move(file));
        }
    }

    IF_HAS(OutputTAP, RetroProject) {
        auto output = std::make_unique<Output>();
        output->type = Output::ZXSpectrumTAP;
        output->location = (locationFactory ? locationFactory->createLocation(ROW(OutputTAP)) : nullptr);
        output->enabled = OPT_STRING(enabled, OutputTAP);

        FOR_EACH(File, OutputTAP) {
            Output::File outputFile = {};
            outputFile.location = (locationFactory ? locationFactory->createLocation(ROW(File)) : nullptr);

            auto ref = OPT_STRING(ref, File);
            auto basic = OPT_STRING(basic, File);
            outputFile.name = OPT_STRING(name, File);

            if (ref && !basic)
                outputFile.ref = std::move(ref);
            else if (basic && !ref)
                outputFile.basic = std::move(basic);
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
        output->location = (locationFactory ? locationFactory->createLocation(ROW(OutputTRD)) : nullptr);
        output->enabled = OPT_STRING(enabled, OutputTRD);

        FOR_EACH(File, OutputTRD) {
            Output::File outputFile = {};
            outputFile.location = (locationFactory ? locationFactory->createLocation(ROW(File)) : nullptr);

            auto ref = OPT_STRING(ref, File);
            auto basic = OPT_STRING(basic, File);
            outputFile.name = OPT_STRING(name, File);

            if (ref && !basic)
                outputFile.ref = std::move(ref);
            else if (basic && !ref)
                outputFile.basic = std::move(basic);
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
    switch (section.attachment) {
        case Project::Section::Attachment::Default: break;
        case Project::Section::Attachment::Lower: ss << " attachment=\"" << "lower" << '"'; break;
        case Project::Section::Attachment::Upper: ss << " attachment=\"" << "upper" << '"'; break;
    }
    switch (section.compression) {
        case Compression::None: break;
        case Compression::Zx7: ss << " compression=\"" << "zx7" << '"'; break;
        case Compression::Zx0: ss << " compression=\"" << "zx0" << '"'; break;
        case Compression::Zx0Quick: ss << " compression=\"" << "zx0-quick" << '"'; break;
        case Compression::Lzsa2: ss << " compression=\"" << "lzsa2" << '"'; break;
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
        if (file.name) {
            ss << " name=";
            xmlEncodeInQuotes(ss, *file.name);
        }
        if (file.ref) {
            ss << " ref=";
            xmlEncodeInQuotes(ss, *file.ref);
        } else if (file.basic) {
            ss << " basic=";
            xmlEncodeInQuotes(ss, *file.basic);
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
    if (outputDirectory) {
        ss << '\n';
        ss << "    <OutputDirectory path=";
        xmlEncodeInQuotes(ss, *outputDirectory);
        ss << " />\n";
    }
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
            for (const auto& section : file->sections)
                writeSection(ss, "Section", *section);
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
        std::filesystem::path projectPath = mPath;
        projectPath.remove_filename();

        std::stringstream ss;
        ss << "/_out\n";
        writeFile(projectPath / ".gitignore", ss.str());
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Project::Output::isEnabled(SymbolTable* symbolTable) const
{
    if (!enabled.has_value())
        return true;

    auto heap = symbolTable->heap();
    ExpressionParser parser(heap, nullptr, nullptr, nullptr);
    Expr* value = parser.tryParseExpression(location, enabled.value().c_str(), symbolTable);
    if (!value) {
        std::stringstream ss;
        ss << "unable to parse value for attribute \"enabled\": " << parser.error();
        throw CompilerError(parser.errorLocation(), ss.str());
    }

    return value->evaluateValue(nullptr, nullptr).number != 0;
}
