#include "drunkfly_Builder.h"
#include "Compiler/Java/JNIStringRef.h"
#include "Compiler/Java/JVMThreadContext.h"
#include "Compiler/Java/Exceptions.h"

static void JNICALL Builder_compilerAddSource(JNIEnv* env, jclass, jstring name, jstring path)
{
    JTRY
        JNIStringRef nameRef{name, JNIRef::Unknown};
        JNIStringRef pathRef{path, JNIRef::Unknown};
        JVMThreadContext::instance()->addGeneratedFile(nameRef, pathRef);
    JCATCH
}

const std::vector<JNINativeMethod> NATIVES_drunkfly_Builder {
        { "compilerAddSource", "(Ljava/lang/String;Ljava/lang/String;)V", (void*)Builder_compilerAddSource },
    };
