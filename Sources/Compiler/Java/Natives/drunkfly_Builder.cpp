#include "Compiler/Java/JavaClasses.h"
#include "Compiler/Java/JNIStringRef.h"
#include "Compiler/Java/JVMThreadContext.h"
#include "Compiler/Java/Exceptions.h"

void JNICALL NATIVE_drunkfly_Builder_compilerAddSource(JNIEnv* env, jclass, jstring name, jstring path)
{
    JTRY
        JNIStringRef nameRef{name, JNIRef::Unknown};
        JNIStringRef pathRef{path, JNIRef::Unknown};
        JVMThreadContext::instance()->addGeneratedFile(nameRef, pathRef);
    JCATCH
}
