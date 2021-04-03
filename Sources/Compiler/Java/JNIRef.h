#ifndef COMPILER_JAVA_JNIREF_H
#define COMPILER_JAVA_JNIREF_H

#include "Compiler/Java/JNI.h"

#define JNIREF(CLASSNAME, TYPE) \
    public: \
    CLASSNAME(TYPE object = nullptr, Type type = Local) : JNIRef(object, type) {} \
    CLASSNAME(JNIRef&& other) : JNIRef(std::move(other)) {} \
    CLASSNAME(CLASSNAME&& other) : JNIRef(std::move(other)) {} \
    ~CLASSNAME() override { release(); } \
    CLASSNAME& operator=(TYPE string) { JNIRef::operator=(string); return *this; } \
    CLASSNAME& operator=(CLASSNAME&& other) { JNIRef::operator=(std::move(other)); return *this; } \
    CLASSNAME& operator=(JNIRef&& other) { JNIRef::operator=(std::move(other)); return *this; } \
    TYPE toJNI() const { return (TYPE)JNIRef::toJNI(); }

class JNIClassRef;

class JNIRef
{
public:
    enum Type : char
    {
        Unknown,
        Local,
        Global,
    };

    JNIRef(jobject object = nullptr, Type type = Local);
    JNIRef(JNIRef&& other);
    virtual ~JNIRef();

    JNIRef& operator=(jobject object);
    JNIRef& operator=(JNIRef&& other);

    virtual void release();

    JNIRef newGlobalRef() const;
    JNIRef newLocalRef() const;
    JNIClassRef getClass() const;

    jobject toJNI() const { return mObjectRef; }

    explicit operator bool() const { return mObjectRef != nullptr; }
    bool operator!() const { return mObjectRef == nullptr; }

    bool operator==(const JNIRef& other) const { return mObjectRef == other.mObjectRef; }
    bool operator!=(const JNIRef& other) const { return mObjectRef != other.mObjectRef; }
    bool operator<(const JNIRef& other) const { return mObjectRef < other.mObjectRef; }

protected:
    jobject mObjectRef;
    Type mType;

    void moveFrom(JNIRef& other);

    DISABLE_COPY(JNIRef);
};

#endif
