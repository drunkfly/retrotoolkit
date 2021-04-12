#include "Label.h"
#include "Compiler/CompilerError.h"

class Label::Address final : public GCObject
{
public:
    Address* next;
    Address* prevRead;

    Address()
        : next(nullptr)
        , prevRead(nullptr)
        , mValue(0)
        , mIsValid(false)
    {
    }

    bool isValid() const
    {
        return mIsValid;
    }

    size_t value() const
    {
        if (!mIsValid) {
            assert(false);
            throw CompilerError(nullptr, "internal compiler error: attempted to get address of unresolved label.");
        }
        return mValue;
    }

    void setValue(size_t value)
    {
        if (mIsValid) {
            assert(false);
            throw CompilerError(nullptr, "internal compiler error: attempted to set address of resolved label.");
        }
        mValue = value;
        mIsValid = true;
    }

    void unsetValue()
    {
        mIsValid = false;
    }

private:
    size_t mValue;
    bool mIsValid;

    DISABLE_COPY(Address);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Label::Label(SourceLocation* location, const char* name)
    : Instruction(location)
    , mName(name)
    , mFirstAddress(new (heap()) Address())
    , mCurrentReadAddress(mFirstAddress)
    , mCurrentWriteAddress(mFirstAddress)
    , mSavedReadAddress(nullptr)
{
    registerFinalizer();
}

Label::~Label()
{
}

Instruction::Type Label::type() const
{
    return Type::Label;
}

bool Label::calculateSizeInBytes(size_t& outSize, ISectionResolver*, std::unique_ptr<CompilerError>&) const
{
    outSize = 0;
    return true;
}

bool Label::canEmitCodeWithoutBaseAddress(ISectionResolver*) const
{
    return true;
}

bool Label::emitCode(CodeEmitter*, int64_t&, ISectionResolver*, std::unique_ptr<CompilerError>&) const
{
    return true;
}

bool Label::hasAddress() const
{
    return mCurrentReadAddress->isValid();
}

Label::Address* Label::address() const
{
    return mCurrentReadAddress;
}

size_t Label::addressValue() const
{
    assert(mCurrentReadAddress && mCurrentReadAddress->isValid());
    return mCurrentReadAddress->value();
}

void Label::setAddress(size_t address)
{
    mCurrentWriteAddress->setValue(address);
}

void Label::unsetAddresses()
{
    for (Address* addr = mFirstAddress; addr; addr = addr->next)
        addr->unsetValue();
}

void Label::resetCounters() const
{
    mCurrentReadAddress = mFirstAddress;
    mCurrentWriteAddress = mFirstAddress;
    mSavedReadAddress = nullptr;
}

void Label::saveReadCounter() const
{
    mCurrentReadAddress->prevRead = mSavedReadAddress;
    mSavedReadAddress = mCurrentReadAddress;
}

void Label::restoreReadCounter() const
{
    mCurrentReadAddress = mSavedReadAddress;
    mSavedReadAddress = mCurrentReadAddress->prevRead;
}

void Label::advanceCounters() const
{
    if (mCurrentWriteAddress->next)
        mCurrentWriteAddress = mCurrentWriteAddress->next;
    else {
        auto addr = new (heap()) Address();
        mCurrentWriteAddress->next = addr;
        mCurrentWriteAddress = addr;
    }

    assert(mCurrentReadAddress->next);
    mCurrentReadAddress = mCurrentReadAddress->next;
}

Instruction* Label::clone() const
{
    return new (heap()) Label(location(), mName);
}
