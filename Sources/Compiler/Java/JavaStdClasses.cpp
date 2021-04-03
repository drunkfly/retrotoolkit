#include "JavaStdClasses.h"
#include "Compiler/Java/JVM.h"
#include "Common/IO.h"

#define JAVA_STD_CLASS(NAME) JNIClassRef JavaStdClasses::NAME;
#include "Compiler/Java/JavaStdClasses.lst"
#undef JAVA_STD_CLASS

void JavaStdClasses::ensureLoaded()
{
    #define JAVA_STD_CLASS(NAME) { \
            char name[] = #NAME; \
            for (char& ch : name) { \
                if (ch == '_') \
                    ch = '/'; \
            } \
            NAME.loadGlobal(name); \
        }
    #include "Compiler/Java/JavaStdClasses.lst"
    #undef JAVA_STD_CLASS
}

void JavaStdClasses::releaseAll()
{
    #define JAVA_STD_CLASS(NAME) NAME.release();
    #include "Compiler/Java/JavaStdClasses.lst"
    #undef JAVA_STD_CLASS
}
