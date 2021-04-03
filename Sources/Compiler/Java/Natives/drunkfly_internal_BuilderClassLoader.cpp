#include "drunkfly_internal_BuilderClassLoader.h"
#include "Compiler/Java/JVMThreadContext.h"
#include "Compiler/Java/Exceptions.h"

static jobject JNICALL BuilderClassLoader_getInstance(JNIEnv* env, jclass)
{
    JTRY
        return JVMThreadContext::instance()->classLoader().toJNI();
    JCATCH
        return nullptr;
}

const std::vector<JNINativeMethod> NATIVES_drunkfly_internal_BuilderClassLoader {
        { "getInstance", "()Ldrunkfly/internal/BuilderClassLoader;", (void*)BuilderClassLoader_getInstance },
    };
