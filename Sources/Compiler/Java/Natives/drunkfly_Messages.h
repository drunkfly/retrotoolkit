#ifndef JAVA_NATIVES_DRUNKFLY_MESSAGES_H
#define JAVA_NATIVES_DRUNKFLY_MESSAGES_H

#include "Compiler/Java/JNI.h"

extern jobject JNICALL drunkfly_Messages_getInstance(JNIEnv* env, jclass);
extern jobject JNICALL drunkfly_Messages_getPrintWriter(JNIEnv* env, jclass);
extern void JNICALL drunkfly_Messages_print(JNIEnv* env, jclass, jstring message);

#endif
