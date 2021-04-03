#ifndef COMPILER_JAVA_JAVASTDCLASSES_H
#define COMPILER_JAVA_JAVASTDCLASSES_H

#include "Compiler/Java/JNIClassRef.h"

class JavaStdClasses
{
public:
    #define JAVA_STD_CLASS(NAME) static JNIClassRef NAME;
    #include "Compiler/Java/JavaStdClasses.lst"
    #undef JAVA_STD_CLASS

    static void ensureLoaded();
    static void releaseAll();
};

#endif
