#ifndef COMPILER_JAVA_JNITHROWABLEREF_H
#define COMPILER_JAVA_JNITHROWABLEREF_H

#include "Compiler/Java/JNIRef.h"

class SourceLocation;

class JNIThrowableRef final : public JNIRef
{
    JNIREF(JNIThrowableRef, jthrowable)

public:
    static JNIThrowableRef catchCurrent();

    void rethrow(SourceLocation* location = nullptr) const;
    static void rethrowCurrentException(SourceLocation* location = nullptr);

    DISABLE_COPY(JNIThrowableRef);
};

#endif
