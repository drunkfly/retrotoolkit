#include "AssemblerContextIf.h"
#include "Compiler/Assembler/MacroIf.h"
#include "Compiler/Assembler/Label.h"
#include "Compiler/Linker/ProgramSection.h"
#include "Compiler/Tree/Expr.h"
#include "Compiler/Tree/SymbolTable.h"
#include "Compiler/Tree/Symbol.h"
#include "Compiler/Token.h"
#include "Compiler/CompilerError.h"

// FIXME: if AssemblerContextIf is empty, its condition is never validated

AssemblerContextIf::AssemblerContextIf(AssemblerContext* prev, const Token* token, Expr* condition)
    : AssemblerContext(prev)
    , mMacro(nullptr)
    , mCondition(condition)
    , mIfToken(token)
    , mElseToken(nullptr)
    , mThenPrefixLocation(nullptr)
    , mElsePrefixLocation(nullptr)
    , mHasElse(false)
{
    registerFinalizer();
    AssemblerContext::setCurrentSection(prev->currentSection());
}

bool AssemblerContextIf::isIf() const
{
    return true;
}

bool AssemblerContextIf::hasElse() const
{
    return mHasElse;
}

void AssemblerContextIf::beginElse(const Token* token)
{
    if (mHasElse)
        throw CompilerError(token->location(), "internal compiler error: beginElse() called inside else block.");
    mHasElse = true;
    mElseToken = token;
}

void AssemblerContextIf::validateAtPop(SourceLocation* location)
{
    if (!mThenPrefixLocation && !mElsePrefixLocation)
        return;

    if (mThenPrefix == mElsePrefix) {
        prev()->setLocalLabelsPrefix(location, mElsePrefix);
        return;
    }

    if (mThenPrefixLocation && mElsePrefixLocation) {
        std::stringstream ss;
        ss << "$if" << allocEphemeralLabelCounter() << "$(" << mThenPrefix << "||" << mElsePrefix << ')';
        prev()->setLocalLabelsPrefix(location, ss.str());
    }

    if (mThenPrefixLocation)
        prev()->setLocalLabelsPrefix(location, mThenPrefix);
    else
        prev()->setLocalLabelsPrefix(location, mElsePrefix);
}

const std::string& AssemblerContextIf::localLabelsPrefix() const
{
    return prev()->localLabelsPrefix();
}

void AssemblerContextIf::setLocalLabelsPrefix(SourceLocation* location, std::string prefix)
{
    if (mHasElse) {
        mElsePrefix = prefix;
        mElsePrefixLocation = location;
    } else {
        mThenPrefix = prefix;
        mThenPrefixLocation = location;
    }

    prev()->setLocalLabelsPrefix(location, std::move(prefix));
}

Expr* AssemblerContextIf::condition() const
{
    Expr* result = mCondition;
    if (mHasElse)
        result = new (heap()) ExprLogicNot(result->location(), result);

    Expr* parent = prev()->condition();
    if (parent)
        result = new (heap()) ExprLogicAnd(result->location(), result, parent);

    return result;
}

bool AssemblerContextIf::setCurrentSection(ProgramSection*)
{
    return false;
}

void AssemblerContextIf::addInstruction(Instruction* instruction)
{
    if (!currentSection())
        throw CompilerError(instruction->location(), "code or data not in a section.");

    if (!mMacro) {
        mMacro = new (heap()) MacroIf(instruction->location(), mCondition);
        prev()->addInstruction(mMacro);
    }

    if (mHasElse)
        mMacro->addElseInstruction(instruction);
    else
        mMacro->addThenInstruction(instruction);
}

void AssemblerContextIf::addConstant(SymbolTable* symbolTable, SourceLocation* location, const char* name, Expr* value)
{
    ConditionalConstantSymbol* constSymbol = nullptr;

    auto symbol = symbolTable->findSymbol(name);
    if (symbol) {
        if (symbol->type() != Symbol::Type::ConditionalConstant) {
            std::stringstream ss;
            ss << "duplicate identifier \"" << name << "\".";
            throw CompilerError(location, ss.str());
        }
        constSymbol = static_cast<ConditionalConstantSymbol*>(symbol);
    }

    if (!constSymbol) {
        constSymbol = new (heap()) ConditionalConstantSymbol(location, name);
        if (!symbolTable->addSymbol(constSymbol)) {
            std::stringstream ss;
            ss << "duplicate identifier \"" << name << "\".";
            throw CompilerError(location, ss.str());
        }
    }

    constSymbol->addValue(condition(), value);
}

void AssemblerContextIf::addLabel(SymbolTable* symbolTable, SourceLocation* location, const char* name, size_t offset)
{
    if (!currentSection())
        throw CompilerError(location, "label not in a section.");

    ConditionalLabelSymbol* labelSymbol = nullptr;

    auto symbol = symbolTable->findSymbol(name);
    if (symbol) {
        if (symbol->type() != Symbol::Type::ConditionalLabel) {
            std::stringstream ss;
            ss << "duplicate identifier \"" << name << "\".";
            throw CompilerError(location, ss.str());
        }
        labelSymbol = static_cast<ConditionalLabelSymbol*>(symbol);
    }

    if (!labelSymbol) {
        labelSymbol = new (heap()) ConditionalLabelSymbol(location, name);
        if (!symbolTable->addSymbol(labelSymbol)) {
            std::stringstream ss;
            ss << "duplicate identifier \"" << name << "\".";
            throw CompilerError(location, ss.str());
        }
    }

    auto label = new (heap()) Label(location, std::move(name), offset);
    labelSymbol->addLabel(condition(), label);

    addInstruction(label);
}
