#include "Project.h"
#include "Compiler/Output/SpectrumSnapshotWriter.h"
#include "Compiler/Tree/Expr.h"
#include "Compiler/Tree/SourceLocation.h"
#include "Compiler/Tree/SourceLocationFactory.h"
#include "Compiler/Tree/Symbol.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/Linker/Program.h"
#include "Compiler/CompilerError.h"
#include "Compiler/ExpressionParser.h"
#include "Common/Xml.h"
#include "Common/IO.h"
#include "Common/Strings.h"

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
    section->condition = OPT_STRING(enableIf, Section);
    section->conditionLocation = (locFactory ? locFactory->createLocation(ATTR_ROW(enableIf, Section)) : nullptr);
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

static std::unique_ptr<Project::Output::Z80> parseZ80(
    const XmlDocument& xml, XmlNode xmlOutputZ80, SourceLocationFactory* locFactory)
{
    auto z80 = std::make_unique<Project::Output::Z80>();

    IF_HAS(Format, OutputZ80) {
        auto loc = (locFactory ? locFactory->createLocation(ROW(Format)) : nullptr);
        z80->format = Project::Output::Z80::Value{ loc, REQ_STRING(value, Format) };
    }

    IF_HAS(Machine, OutputZ80) {
        auto loc = (locFactory ? locFactory->createLocation(ROW(Machine)) : nullptr);
        z80->machine = Project::Output::Z80::Value{ loc, REQ_STRING(value, Machine) };
    }

    FOR_EACH(Register, OutputZ80) {
        std::string name = toLower(REQ_STRING(name, Register));
        const std::string& value = REQ_STRING(value, Register);
        auto loc = (locFactory ? locFactory->createLocation(ROW(Register)) : nullptr);

        if (name == "a") z80->a = Project::Output::Z80::Value{ loc, std::move(value) };
        else if (name == "f") z80->f = Project::Output::Z80::Value{ loc, std::move(value) };
        else if (name == "bc") z80->bc = Project::Output::Z80::Value{ loc, std::move(value) };
        else if (name == "hl") z80->hl = Project::Output::Z80::Value{ loc, std::move(value) };
        else if (name == "de") z80->de = Project::Output::Z80::Value{ loc, std::move(value) };
        else if (name == "a'") z80->shadowA = Project::Output::Z80::Value{ loc, std::move(value) };
        else if (name == "f'") z80->shadowF = Project::Output::Z80::Value{ loc, std::move(value) };
        else if (name == "bc'") z80->shadowBC = Project::Output::Z80::Value{ loc, std::move(value) };
        else if (name == "hl'") z80->shadowHL = Project::Output::Z80::Value{ loc, std::move(value) };
        else if (name == "de'") z80->shadowDE = Project::Output::Z80::Value{ loc, std::move(value) };
        else if (name == "pc") z80->pc = Project::Output::Z80::Value{ loc, std::move(value) };
        else if (name == "sp") z80->sp = Project::Output::Z80::Value{ loc, std::move(value) };
        else if (name == "iy") z80->iy = Project::Output::Z80::Value{ loc, std::move(value) };
        else if (name == "ix") z80->ix = Project::Output::Z80::Value{ loc, std::move(value) };
        else if (name == "i") z80->i = Project::Output::Z80::Value{ loc, std::move(value) };
        else if (name == "r") z80->r = Project::Output::Z80::Value{ loc, std::move(value) };
        else {
            auto loc = (locFactory ? locFactory->createLocation(ROW(Register)) : nullptr);
            std::stringstream ss;
            ss << "invalid register name \"" << name << "\".";
            throw CompilerError(loc, ss.str());
        }
    }

    FOR_EACH(Port, OutputZ80) {
        std::string number = toLower(REQ_STRING(number, Port));
        const std::string& value = REQ_STRING(value, Port);
        auto loc = (locFactory ? locFactory->createLocation(ROW(Port)) : nullptr);

        if (number == "1ffd") z80->port1FFD = Project::Output::Z80::Value{ loc, value };
        else if (number == "7ffd") z80->port7FFD = Project::Output::Z80::Value{ loc, value };
        else if (number == "fffd") z80->portFFFD = Project::Output::Z80::Value{ loc, value };
        else {
            auto loc = (locFactory ? locFactory->createLocation(ROW(Port)) : nullptr);
            std::stringstream ss;
            ss << "invalid port number \"" << REQ_STRING(number, Port) << "\".";
            throw CompilerError(loc, ss.str());
        }
    }

    IF_HAS(BorderColor, OutputZ80) {
        auto loc = (locFactory ? locFactory->createLocation(ROW(BorderColor)) : nullptr);
        z80->borderColor = Project::Output::Z80::Value{ loc, REQ_STRING(value, BorderColor) };
    }

    IF_HAS(InterruptMode, OutputZ80) {
        auto loc = (locFactory ? locFactory->createLocation(ROW(InterruptMode)) : nullptr);
        z80->interruptMode = Project::Output::Z80::Value{ loc, REQ_STRING(value, InterruptMode) };
    }

    IF_HAS(InterruptsEnabled, OutputZ80) {
        auto loc = (locFactory ? locFactory->createLocation(ROW(InterruptsEnabled)) : nullptr);
        z80->interruptsEnabled = Project::Output::Z80::Value{ loc, REQ_STRING(value, InterruptsEnabled) };
    }

    return z80;
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

    FOR_EACH(OutputTAP, RetroProject) {
        auto output = std::make_unique<Output>();
        output->type = Output::ZXSpectrumTAP;
        output->location = (locationFactory ? locationFactory->createLocation(ROW(OutputTAP)) : nullptr);
        output->enabled = OPT_STRING(enabled, OutputTAP);

        FOR_EACH(File, OutputTAP) {
            Output::File outputFile = {};
            outputFile.location = (locationFactory ? locationFactory->createLocation(ROW(File)) : nullptr);
            outputFile.name = OPT_STRING(name, File);
            outputFile.ref = OPT_STRING(ref, File);
            outputFile.refBasic = OPT_STRING(refBasic, File);
            output->files.emplace_back(std::move(outputFile));
        }

        outputs.emplace_back(std::move(output));
    }

    FOR_EACH(OutputTRD, RetroProject) {
        auto output = std::make_unique<Output>();
        output->type = Output::ZXSpectrumTRD;
        output->location = (locationFactory ? locationFactory->createLocation(ROW(OutputTRD)) : nullptr);
        output->enabled = OPT_STRING(enabled, OutputTRD);

        FOR_EACH(File, OutputTRD) {
            Output::File outputFile = {};
            outputFile.location = (locationFactory ? locationFactory->createLocation(ROW(File)) : nullptr);
            outputFile.name = OPT_STRING(name, File);
            outputFile.ref = OPT_STRING(ref, File);
            outputFile.refBasic = OPT_STRING(refBasic, File);
            output->files.emplace_back(std::move(outputFile));
        }

        outputs.emplace_back(std::move(output));
    }

    FOR_EACH(OutputZ80, RetroProject) {
        auto output = std::make_unique<Output>();
        output->type = Output::ZXSpectrumZ80;
        output->location = (locationFactory ? locationFactory->createLocation(ROW(OutputZ80)) : nullptr);
        output->enabled = OPT_STRING(enabled, OutputZ80);
        output->z80 = parseZ80(xml, xmlOutputZ80, locationFactory);

        FOR_EACH(File, OutputZ80) {
            Output::File outputFile = {};
            outputFile.location = (locationFactory ? locationFactory->createLocation(ROW(File)) : nullptr);
            outputFile.name = OPT_STRING(name, File);
            outputFile.ref = OPT_STRING(ref, File);
            output->files.emplace_back(std::move(outputFile));
        }

        outputs.emplace_back(std::move(output));
    }

    FOR_EACH(OutputPC, RetroProject) {
        auto output = std::make_unique<Output>();
        output->type = Output::PC;
        output->location = (locationFactory ? locationFactory->createLocation(ROW(OutputPC)) : nullptr);
        output->enabled = OPT_STRING(enabled, OutputPC);
        output->z80 = parseZ80(xml, xmlOutputPC, locationFactory);

        FOR_EACH(File, OutputPC) {
            Output::File outputFile = {};
            outputFile.location = (locationFactory ? locationFactory->createLocation(ROW(File)) : nullptr);
            outputFile.name = OPT_STRING(name, File);
            outputFile.ref = OPT_STRING(ref, File);
            output->files.emplace_back(std::move(outputFile));
        }

        outputs.emplace_back(std::move(output));
    }

    xmlCheckAllAccessed(xml);
}

static void writeSection(std::stringstream& ss, const char* element, const Project::Section& section)
{
    ss << "            <Section name=";
    xmlEncodeInQuotes(ss, section.name);
    if (section.condition) {
        ss << " enableIf=";
        xmlEncodeInQuotes(ss, *section.condition);
    }
    if (section.base) {
        ss << " base=";
        xmlEncodeInQuotes(ss, *section.base);
    }
    if (section.alignment) {
        ss << " alignment=";
        xmlEncodeInQuotes(ss, *section.alignment);
    }
    if (section.fileOffset) {
        ss << " fileOffset=";
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
        case Project::Output::ZXSpectrumZ80: element = "OutputZ80"; break;
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
        }
        if (file.refBasic) {
            ss << " refBasic=";
            xmlEncodeInQuotes(ss, *file.refBasic);
        }
        ss << " />\n";
    }

    if (output.z80) {
        if (output.z80->format.value.has_value())
            ss << "        <Format value=\"" << *output.z80->format.value << "\" />";
        if (output.z80->machine.value.has_value())
            ss << "        <Machine value=\"" << *output.z80->machine.value << "\" />";

        if (output.z80->a.value.has_value())
            ss << "        <Register name=\"A\" value=\"" << *output.z80->a.value << "\" />";
        if (output.z80->f.value.has_value())
            ss << "        <Register name=\"F\" value=\"" << *output.z80->f.value << "\" />";
        if (output.z80->bc.value.has_value())
            ss << "        <Register name=\"BC\" value=\"" << *output.z80->bc.value << "\" />";
        if (output.z80->hl.value.has_value())
            ss << "        <Register name=\"HL\" value=\"" << *output.z80->hl.value << "\" />";
        if (output.z80->de.value.has_value())
            ss << "        <Register name=\"DE\" value=\"" << *output.z80->de.value << "\" />";
        if (output.z80->shadowA.value.has_value())
            ss << "        <Register name=\"A'\" value=\"" << *output.z80->shadowA.value << "\" />";
        if (output.z80->shadowF.value.has_value())
            ss << "        <Register name=\"F'\" value=\"" << *output.z80->shadowF.value << "\" />";
        if (output.z80->shadowBC.value.has_value())
            ss << "        <Register name=\"BC'\" value=\"" << *output.z80->shadowBC.value << "\" />";
        if (output.z80->shadowHL.value.has_value())
            ss << "        <Register name=\"HL'\" value=\"" << *output.z80->shadowHL.value << "\" />";
        if (output.z80->shadowDE.value.has_value())
            ss << "        <Register name=\"DE'\" value=\"" << *output.z80->shadowDE.value << "\" />";
        if (output.z80->pc.value.has_value())
            ss << "        <Register name=\"PC\" value=\"" << *output.z80->pc.value << "\" />";
        if (output.z80->sp.value.has_value())
            ss << "        <Register name=\"SP\" value=\"" << *output.z80->sp.value << "\" />";
        if (output.z80->iy.value.has_value())
            ss << "        <Register name=\"IY\" value=\"" << *output.z80->iy.value << "\" />";
        if (output.z80->ix.value.has_value())
            ss << "        <Register name=\"IX\" value=\"" << *output.z80->ix.value << "\" />";
        if (output.z80->i.value.has_value())
            ss << "        <Register name=\"I\" value=\"" << *output.z80->i.value << "\" />";
        if (output.z80->r.value.has_value())
            ss << "        <Register name=\"R\" value=\"" << *output.z80->r.value << "\" />";

        if (output.z80->port1FFD.value.has_value())
            ss << "        <Port number=\"1ffd\" value=\"" << *output.z80->port1FFD.value << "\" />";
        if (output.z80->port7FFD.value.has_value())
            ss << "        <Port number=\"7ffd\" value=\"" << *output.z80->port7FFD.value << "\" />";
        if (output.z80->portFFFD.value.has_value())
            ss << "        <Port number=\"fffd\" value=\"" << *output.z80->portFFFD.value << "\" />";

        if (output.z80->borderColor.value.has_value())
            ss << "        <BorderColor value=\"" << *output.z80->borderColor.value << "\" />";
        if (output.z80->interruptMode.value.has_value())
            ss << "        <InterruptMode value=\"" << *output.z80->interruptMode.value << "\" />";
        if (output.z80->interruptsEnabled.value.has_value())
            ss << "        <InterruptsEnabled value=\"" << *output.z80->interruptsEnabled.value << "\" />";
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Expr* Project::Output::Z80::Value::parseExpression(Program* program) const
{
    ExpressionParser parser(program->heap(), nullptr, nullptr, nullptr);
    Expr* expr = parser.tryParseExpression(location, value.value().c_str(), program->globals());
    if (!expr) {
        std::stringstream ss;
        ss << "unable to parse expression \"" << value.value() << "\": " << parser.error();
        throw CompilerError(parser.errorLocation(), ss.str());
    }
    return expr;
}

uint8_t Project::Output::Z80::Value::evaluateByte(Program* program, ISectionResolver* sectionResolver) const
{
    return parseExpression(program)->evaluateByte(nullptr, sectionResolver);
}

uint16_t Project::Output::Z80::Value::evaluateWord(Program* program, ISectionResolver* sectionResolver) const
{
    return parseExpression(program)->evaluateWord(nullptr, sectionResolver);
}

bool Project::Output::Z80::Value::evaluateBool(Program* program, ISectionResolver* sectionResolver) const
{
    return evaluateValue(program, sectionResolver).number != 0;
}

::Value Project::Output::Z80::Value::evaluateValue(Program* program, ISectionResolver* sectionResolver) const
{
    return parseExpression(program)->evaluateValue(nullptr, sectionResolver);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Project::Output::Z80::initWriter(
    Program* program, ISectionResolver* sectionResolver, SpectrumSnapshotWriter* writer) const
{
    if (format.value.has_value()) {
        if (*format.value == "auto")
            writer->setFormat(Z80Format::Auto);
        else {
            auto value = format.evaluateValue(program, sectionResolver).number;
            if (value == 1)
                writer->setFormat(Z80Format::Version1);
            else if (value == 2)
                writer->setFormat(Z80Format::Version2);
            else if (value == 3)
                writer->setFormat(Z80Format::Version3);
            else
                throw CompilerError(format.location, "invalid Z80 format.");
        }
    }

    if (machine.value.has_value()) {
        if (*machine.value == "auto")
            writer->setMachine(Z80Machine::Auto);
        else if (*machine.value == "16k")
            writer->setMachine(Z80Machine::Spectrum16k);
        else if (*machine.value == "48k")
            writer->setMachine(Z80Machine::Spectrum48k);
        else if (*machine.value == "128k")
            writer->setMachine(Z80Machine::Spectrum128k);
        else if (*machine.value == "+3")
            writer->setMachine(Z80Machine::SpectrumPlus3);
        else if (*machine.value == "pentagon")
            writer->setMachine(Z80Machine::Pentagon);
        else if (*machine.value == "scorpion")
            writer->setMachine(Z80Machine::Scorpion);
        else if (*machine.value == "didaktik")
            writer->setMachine(Z80Machine::DidaktikKompakt);
        else if (*machine.value == "+2")
            writer->setMachine(Z80Machine::SpectrumPlus2);
        else if (*machine.value == "+2A")
            writer->setMachine(Z80Machine::SpectrumPlus2A);
        else if (*machine.value == "tc2048")
            writer->setMachine(Z80Machine::TC2048);
        else if (*machine.value == "tc2068")
            writer->setMachine(Z80Machine::TC2068);
        else if (*machine.value == "ts2068")
            writer->setMachine(Z80Machine::TS2068);
        else
            throw CompilerError(machine.location, "invalid Z80 machine.");
    }

    if (a.value.has_value())
        writer->setA(a.evaluateByte(program, sectionResolver));
    if (f.value.has_value())
        writer->setF(f.evaluateByte(program, sectionResolver));
    if (bc.value.has_value())
        writer->setBC(bc.evaluateWord(program, sectionResolver));
    if (hl.value.has_value())
        writer->setHL(hl.evaluateWord(program, sectionResolver));
    if (de.value.has_value())
        writer->setDE(de.evaluateWord(program, sectionResolver));
    if (shadowA.value.has_value())
        writer->setShadowA(shadowA.evaluateByte(program, sectionResolver));
    if (shadowF.value.has_value())
        writer->setShadowF(shadowF.evaluateByte(program, sectionResolver));
    if (shadowBC.value.has_value())
        writer->setShadowBC(shadowBC.evaluateWord(program, sectionResolver));
    if (shadowHL.value.has_value())
        writer->setShadowHL(shadowHL.evaluateWord(program, sectionResolver));
    if (shadowDE.value.has_value())
        writer->setShadowDE(shadowDE.evaluateWord(program, sectionResolver));
    if (pc.value.has_value())
        writer->setPC(pc.evaluateWord(program, sectionResolver));
    if (sp.value.has_value())
        writer->setSP(sp.evaluateWord(program, sectionResolver));
    if (iy.value.has_value())
        writer->setIY(iy.evaluateWord(program, sectionResolver));
    if (ix.value.has_value())
        writer->setIX(ix.evaluateWord(program, sectionResolver));
    if (i.value.has_value())
        writer->setI(i.evaluateByte(program, sectionResolver));
    if (r.value.has_value())
        writer->setR(r.evaluateByte(program, sectionResolver));

    if (port1FFD.value.has_value())
        writer->setPort1FFD(port1FFD.evaluateWord(program, sectionResolver));
    if (port7FFD.value.has_value())
        writer->setPort7FFD(port7FFD.evaluateWord(program, sectionResolver));
    if (portFFFD.value.has_value())
        writer->setPortFFFD(portFFFD.evaluateWord(program, sectionResolver));

    if (borderColor.value.has_value()) {
        auto color = borderColor.evaluateValue(program, sectionResolver).number;
        if (color < 0 || color > 7)
            throw CompilerError(borderColor.location, "invalid border color.");
        writer->setBorderColor(uint8_t(color));
    }

    if (interruptMode.value.has_value()) {
        auto color = interruptMode.evaluateValue(program, sectionResolver).number;
        if (color < 0 || color > 2)
            throw CompilerError(interruptMode.location, "invalid interrupt mode.");
        writer->setInterruptMode(uint8_t(color));
    }

    if (interruptsEnabled.value.has_value())
        writer->setInterruptsEnabled(interruptsEnabled.evaluateBool(program, sectionResolver));
}
