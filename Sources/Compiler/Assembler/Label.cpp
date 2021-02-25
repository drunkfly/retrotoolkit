#include "Label.h"
#include "Compiler/CompilerError.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Label::Address : public GCObject
{
public:
    Address() = default;
    virtual ~Address() = default;

    virtual bool isValid() const = 0;
    virtual size_t value() const = 0;
    virtual void setValue(size_t value) = 0;
    virtual void unsetValue() = 0;

protected:
    virtual Address* clone() const = 0;

    friend class Label;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Label::SimpleAddress final : public Address
{
public:
    SimpleAddress()
        : mValue(0)
        , mIsValid(false)
    {
    }

    SimpleAddress(const SimpleAddress& other)
        : mValue(other.mValue)
        , mIsValid(other.mIsValid)
    {
    }

    Address* clone() const final override
    {
        return new (heap()) SimpleAddress(*this);
    }

    bool isValid() const final override
    {
        return mIsValid;
    }

    size_t value() const final override
    {
        if (!mIsValid) {
            assert(false);
            throw CompilerError(nullptr, "internal compiler error: attempted to get address of unresolved label.");
        }
        return mValue;
    }

    void setValue(size_t value) final override
    {
        if (mIsValid) {
            assert(false);
            throw CompilerError(nullptr, "internal compiler error: attempted to set address of resolved label.");
        }
        mValue = value;
        mIsValid = true;
    }

    void unsetValue() final override
    {
        mIsValid = false;
    }

private:
    size_t mValue;
    bool mIsValid;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Label::Label(SourceLocation* location, std::string name)
    : Instruction(location)
    , mName(std::move(name))
    , mAddress(new (heap()) SimpleAddress())
{
    registerFinalizer();
}

Label::~Label()
{
}

bool Label::isLabel() const
{
    return true;
}

size_t Label::sizeInBytes() const
{
    return 0;
}

bool Label::hasAddress() const
{
    return mAddress->isValid();
}

Label::Address* Label::address() const
{
    return mAddress;
}

void Label::setAddress(size_t address)
{
    mAddress->setValue(address);
}

void Label::unsetAddress()
{
    mAddress->unsetValue();
}
