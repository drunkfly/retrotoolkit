#include "Compiler/Java/JavaClasses.h"
#include "Common/Strings.h"
#include "Compiler/Java/Exceptions.h"
#include "Compiler/Java/JVMThreadContext.h"
#include "Compiler/Java/JVMGlobalContext.h"
#include "Compiler/Java/JNIStringRef.h"
#include "Compiler/Java/JVM.h"
#include "Compiler/Compiler.h"

jobject JNICALL NATIVE_drunkfly_Messages_getInstance(JNIEnv* env, jclass)
{
    JTRY
        return JVMGlobalContext::instance()->outputWriter().toJNI();
    JCATCH
        return nullptr;
}

jobject JNICALL NATIVE_drunkfly_Messages_getPrintWriter(JNIEnv* env, jclass)
{
    JTRY
        return JVMGlobalContext::instance()->printWriter().toJNI();
    JCATCH
        return nullptr;
}

void JNICALL NATIVE_drunkfly_Messages_print(JNIEnv* env, jclass, jstring message)
{
    JTRY
        auto listener = JVMThreadContext::instance()->listener();
        if (listener)
            listener->printMessage(JNIStringRef(message, JNIRef::Unknown).toUtf8());
    JCATCH
}
