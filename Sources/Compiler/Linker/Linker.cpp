#include "Linker.h"
#include "Compiler/Tree/SourceLocation.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/Linker/CompiledOutput.h"
#include "Compiler/Project.h"
#include "Compiler/ExpressionParser.h"
#include "Compiler/CompilerError.h"
#include <sstream>

Linker::Linker(GCHeap* heap, const Project* project, std::string projectConfiguration)
    : mHeap(heap)
    , mProject(project)
    , mProjectConfiguration(std::move(projectConfiguration))
{
}

Linker::~Linker()
{
}

static Expr* parseExpression(const Project* project, SymbolTable* symbolTable, const std::string& str)
{
    auto heap = symbolTable->heap();
    auto fileID = new (heap) FileID(project->path().filename(), project->path());
    auto location = new (heap) SourceLocation(fileID, 1);

    ExpressionParser parser(heap, nullptr, nullptr);
    Expr* expr = parser.tryParseExpression(location, str.c_str(), symbolTable);
    if (!expr) {
        std::stringstream ss;
        ss << "unable to parse expression \"" << str << "\": " << parser.error();
        throw CompilerError(parser.errorLocation(), ss.str());
    }

    return expr;
}

static Expr* tryParseExpression(const Project* project, SymbolTable* symbolTable, const std::optional<std::string>& str)
{
    if (!str)
        return nullptr;
    return parseExpression(project, symbolTable, *str);
}

CompiledOutput* Linker::link(Program* program)
{
    auto output = new (mHeap) CompiledOutput();

    mProgram = program;

    auto projectVariables = new (mHeap) SymbolTable(nullptr);
    mProject->setVariables(projectVariables, mProjectConfiguration);

    for (const auto& file : mProject->files) {
        Expr* fileStart = tryParseExpression(mProject, projectVariables, file->start);
        Expr* fileUntil = tryParseExpression(mProject, projectVariables, file->until);



        for (const auto& section : file->lowerSections) {
        }
    }

    return output;
}
