#include "drunkfly_internal_BuilderClassLoader.h"
#include "Compiler/Java/JVMThreadContext.h"
#include "Compiler/Java/Exceptions.h"

jobject JNICALL drunkfly_internal_BuilderClassLoader_getInstance(JNIEnv* env, jclass)
{
    JTRY
        return JVMThreadContext::instance()->classLoader().toJNI();
    JCATCH
        return nullptr;
}
