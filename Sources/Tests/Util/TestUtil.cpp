#include "TestUtil.h"
#include "Compiler/Linker/Program.h"
#include "Compiler/Linker/Linker.h"
#include "Compiler/Assembler/AssemblerParser.h"
#include "Compiler/Lexer.h"
#include "Compiler/Project.h"

static GCHeap heap;

static std::unique_ptr<Project> loadProject(const std::filesystem::path& path)
{
    auto project = std::make_unique<Project>();
    project->load(std::filesystem::path(__FILE__).parent_path().parent_path() / "Data" / path, nullptr);
    return project;
}

static void assemble(Program* program, const char* name, const char* source)
{
    auto fileID = new (&heap) FileID(name, name);
    Lexer lexer(&heap, Lexer::Mode::Assembler);
    lexer.scan(fileID, source);
    AssemblerParser parser(&heap, program);
    parser.parse(lexer.firstToken());
}

static DataBlob link(const std::unique_ptr<Project>& project, Program* program)
{
    Linker linker(&heap, project.get());
    auto output = linker.link(program);

    auto mainFile = output->getFile("MAIN");
    DataBlob result(mainFile);

    for (const auto& file : output->files()) {
        if (file->name() != "MAIN")
            result.setFileData(file->name(), DataBlob(file));
    }

    return result;
}

DataBlob assemble(ErrorConsumer& errorConsumer, const char* source)
{
    try {
        auto program = new (&heap) Program();
        auto project = loadProject("DefaultProject.xml");
        assemble(program, "source", source);
        return link(project, program);
    } catch (const CompilerError& error) {
        errorConsumer.setError(error);
        return DataBlob();
    }
}

DataBlob assemble2(ErrorConsumer& errorConsumer, const char* source1, const char* source2)
{
    try {
        auto program = new (&heap) Program();
        auto project = loadProject("DefaultProject.xml");
        assemble(program, "source1", source1);
        assemble(program, "source2", source2);
        return link(project, program);
    } catch (const CompilerError& error) {
        errorConsumer.setError(error);
        return DataBlob();
    }
}

DataBlob assemble3(ErrorConsumer& errorConsumer, const char* source1, const char* source2, const char* source3)
{
    try {
        auto program = new (&heap) Program();
        auto project = loadProject("DefaultProject.xml");
        assemble(program, "source1", source1);
        assemble(program, "source2", source2);
        assemble(program, "source3", source3);
        return link(project, program);
    } catch (const CompilerError& error) {
        errorConsumer.setError(error);
        return DataBlob();
    }
}
