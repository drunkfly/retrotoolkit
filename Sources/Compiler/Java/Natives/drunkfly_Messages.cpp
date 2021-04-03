#include "drunkfly_Messages.h"
#include "Common/Strings.h"
#include "Compiler/Java/Exceptions.h"
#include "Compiler/Java/JVMThreadContext.h"
#include "Compiler/Java/JVMGlobalContext.h"
#include "Compiler/Java/JNIStringRef.h"
#include "Compiler/Java/JVM.h"
#include "Compiler/Compiler.h"

static jobject JNICALL Messages_getInstance(JNIEnv* env, jclass)
{
    JTRY
        return JVMGlobalContext::instance()->outputWriter().toJNI();
    JCATCH
        return nullptr;
}

static jobject JNICALL Messages_getPrintWriter(JNIEnv* env, jclass)
{
    JTRY
        return JVMGlobalContext::instance()->printWriter().toJNI();
    JCATCH
        return nullptr;
}

static void JNICALL Messages_print(JNIEnv* env, jclass, jstring message)
{
    JTRY
        auto listener = JVMThreadContext::instance()->listener();
        if (listener)
            listener->printMessage(JNIStringRef(message, JNIRef::Unknown).toUtf8());
    JCATCH
}

const std::vector<JNINativeMethod> NATIVES_drunkfly_Messages {
        { "getInstance", "()Ldrunkfly/Messages;", (void*)Messages_getInstance },
        { "getPrintWriter", "()Ljava/io/PrintWriter;", (void*)Messages_getPrintWriter },
        { "print", "(Ljava/lang/String;)V", (void*)Messages_print },
    };
