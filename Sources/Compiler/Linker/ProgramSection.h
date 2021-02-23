#ifndef COMPILER_LINKER_PROGRAMSECTION_H
#define COMPILER_LINKER_PROGRAMSECTION_H

#include "Common/Common.h"
#include "Common/GC.h"
#include "Compiler/Compression/Compression.h"
#include <vector>
#include <string>

class Instruction;

class ProgramSection : public GCObject
{
public:
    explicit ProgramSection(std::string name);
    ~ProgramSection() override;

    const std::string& name() const { return mName; }

    /*
    bool hasBase() const { return mBase != nullptr; }
    void setBase(Expr* expr);
    unsigned base() const;

    bool hasFileOffset() const { return mFileOffset != nullptr; }
    void setFileOffset(Expr* expr);
    unsigned fileOffset() const;

    bool hasAlignment() const { return mAlignment != nullptr; }
    void setAlignment(Expr* expr);
    unsigned alignment() const;

    Compression compression() const { return mCompression; }
    bool isCompressed() const { return mCompression != Compression::None; }
    void setCompression(Compression compression) { mCompression = compression; }

    unsigned resolvedBase() const { return mResolvedBase; }
    bool resolveAddresses(IErrorReporter* reporter, Program* program, quint32& address, bool compressed) const override;
    size_t emitCode(Program* program, IProgramBinary* binary, IErrorReporter* reporter) const override;
    */

    void addInstruction(Instruction* instruction);

private:
    /*
    Program* mProgram;
    Token mToken;
    Compression mCompression;
    */
    std::string mName;
    std::vector<Instruction*> mInstructions;
    /*
    std::unique_ptr<Expression> mBase;
    std::unique_ptr<Expression> mAlignment;
    std::string mFileName;
    mutable unsigned mResolvedBase;
    mutable unsigned mCalculatedBase;
    mutable unsigned mCalculatedAlignment;
    mutable bool mHasCalculatedBase;
    mutable bool mHasCalculatedAlignment;
    unsigned mBankAddress;
    bool mHasFileName;
    bool mIsImaginary;
    */

    DISABLE_COPY(ProgramSection);
};

#endif
