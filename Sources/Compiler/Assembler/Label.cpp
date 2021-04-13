#include "Label.h"
#include "Compiler/CompilerError.h"

//#define DEBUG_LABEL 1

#if defined(_WIN32) && defined(DEBUG_LABEL) && !defined(NDEBUG)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Label::Address final : public GCObject
{
public:
    Address* next;
  #if defined(DEBUG_LABEL) && !defined(NDEBUG)
    int index = 0;
  #endif

    Address()
        : next(nullptr)
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

class Label::StackEntry final : public GCObject
{
public:
    StackEntry* prev = nullptr;
    Address* address = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Label::Label(SourceLocation* location, const char* name)
    : Instruction(location)
    , mName(name)
    , mFirstAddress(new (heap()) Address())
    , mCurrentReadAddress(mFirstAddress)
    , mCurrentWriteAddress(mFirstAddress)
    , mSavedReadAddresses(nullptr)
{
    registerFinalizer();

  #if defined(DEBUG_LABEL) && !defined(NDEBUG)
    mFirstAddress->index = 0;
  #endif
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
  #if defined(_WIN32) && defined(DEBUG_LABEL) && !defined(NDEBUG)
    { std::stringstream ss;
    ss << "set address " << mCurrentWriteAddress->index << " of "
       << mName << ":" << location()->line() << " to " << address << "\n";
    OutputDebugStringA(ss.str().c_str()); }
  #endif

    mCurrentWriteAddress->setValue(address);
}

void Label::unsetAddresses()
{
    for (Address* addr = mFirstAddress; addr; addr = addr->next)
        addr->unsetValue();

  #if defined(_WIN32) && defined(DEBUG_LABEL) && !defined(NDEBUG)
    { std::stringstream ss;
    ss << "unset addresses of " << mName << ":" << location()->line() << "\n";
    OutputDebugStringA(ss.str().c_str()); }
  #endif
}

void Label::resetCounters() const
{
    mCurrentReadAddress = mFirstAddress;
    mCurrentWriteAddress = mFirstAddress;
    mSavedReadAddresses = nullptr;

  #if defined(_WIN32) && defined(DEBUG_LABEL) && !defined(NDEBUG)
    { std::stringstream ss;
    ss << "reset counters of " << mName << ":" << location()->line() << "\n";
    OutputDebugStringA(ss.str().c_str()); }
  #endif
}

void Label::saveReadCounter() const
{
    auto entry = new (heap()) StackEntry;
    entry->prev = mSavedReadAddresses;
    entry->address = mCurrentReadAddress;
    mSavedReadAddresses = entry;

  #if defined(_WIN32) && defined(DEBUG_LABEL) && !defined(NDEBUG)
    { std::stringstream ss;
    ss << "save read counter " << mCurrentReadAddress->index << " of " << mName << ":" << location()->line() << "\n";
    OutputDebugStringA(ss.str().c_str()); }
  #endif
}

void Label::restoreReadCounter() const
{
    assert(mSavedReadAddresses != nullptr);
    if (!mSavedReadAddresses)
        throw CompilerError(location(), "internal compiler error: read counter stack is empty.");

  #if defined(_WIN32) && defined(DEBUG_LABEL) && !defined(NDEBUG)
    { std::stringstream ss;
    ss << "restore read counter from " << mCurrentReadAddress->index << " to "
       << mSavedReadAddresses->address->index << " of " << mName << ":" << location()->line() << "\n";
    OutputDebugStringA(ss.str().c_str()); }
  #endif

    mCurrentReadAddress = mSavedReadAddresses->address;
    mSavedReadAddresses = mSavedReadAddresses->prev;
}

void Label::advanceCounters() const
{
    if (mCurrentWriteAddress->next) {
        mCurrentWriteAddress = mCurrentWriteAddress->next;

      #if defined(_WIN32) && defined(DEBUG_LABEL) && !defined(NDEBUG)
        { std::stringstream ss;
        ss << "advance write counter to " << mCurrentWriteAddress->index << " of "
           << mName << ":" << location()->line() << "\n";
        OutputDebugStringA(ss.str().c_str()); }
      #endif
    } else {
        auto addr = new (heap()) Address();

      #if defined(_WIN32) && defined(DEBUG_LABEL) && !defined(NDEBUG)
        addr->index = mCurrentWriteAddress->index + 1;
        { std::stringstream ss;
        ss << "advance write counter to " << addr->index << " of " << mName << ":" << location()->line() << "\n";
        OutputDebugStringA(ss.str().c_str()); }
      #endif

        mCurrentWriteAddress->next = addr;
        mCurrentWriteAddress = addr;
    }

    assert(mCurrentReadAddress->next);
    mCurrentReadAddress = mCurrentReadAddress->next;

  #if defined(_WIN32) && defined(DEBUG_LABEL) && !defined(NDEBUG)
    { std::stringstream ss;
    ss << "advance read counter to " << mCurrentReadAddress->index << " of "
       << mName << ":" << location()->line() << "\n";
    OutputDebugStringA(ss.str().c_str()); }
  #endif
}

Instruction* Label::clone() const
{
    return new (heap()) Label(location(), mName);
}
