#ifndef JNI_H
#define JNI_H

#include <stdarg.h>
#include <stdint.h>

typedef uint8_t jboolean;
typedef uint16_t jchar;
typedef int8_t jbyte;
typedef int16_t jshort;
typedef int32_t jint;
typedef int64_t jlong;
typedef float jfloat;
typedef double jdouble;
typedef void* jobject;
typedef void* jweak;
typedef void* jclass;
typedef void* jmethodID;
typedef void* jfieldID;
typedef void* jarray;
typedef void* jbooleanArray;
typedef void* jcharArray;
typedef void* jbyteArray;
typedef void* jshortArray;
typedef void* jintArray;
typedef void* jlongArray;
typedef void* jfloatArray;
typedef void* jdoubleArray;
typedef void* jobjectArray;
typedef void* jstring;
typedef void* jthrowable;

enum : jboolean
{
    JNI_FALSE = 0,
    JNI_TRUE = 1,
};

enum : jint
{
    JNI_VERSION_1_4 = (jint)0x00010004L,

    JNI_OK = 0,
    JNI_ERR = -1,
    JNI_EDETACHED = -2,
    JNI_EVERSION = -3,
    JNI_ENOMEM = -4,
    JNI_EEXIST = -5,
    JNI_EINVAL = -6,
};

union jvalue
{
    jboolean z;
    jchar c;
    jbyte b;
    jshort s;
    jint i;
    jlong j;
    jfloat f;
    jdouble d;
    jobject l;
};

struct JNINativeMethod
{
    const char* name;
    const char* signature;
    const void* fnPtr;
};

struct JavaVMOption
{
    const char* optionString;
    void* extraInfo;
};

struct JavaVMInitArgs
{
    jint version;
    jint nOptions;
    const JavaVMOption* options;
    jboolean ignoreUnrecognized;
};

struct JavaVM;
struct JNIEnv;

#ifdef _WIN32
#define JNICALL __stdcall
#else
#define JNICALL
#endif

#define JNIFUNC(NAME) (JNICALL* NAME)

#define JNI_STRUCT_BEGIN(NAME, UNUSED) struct NAME { const struct { void* unused[UNUSED];
#define JNI_STRUCT_END() }* vtbl; };

typedef jint (JNICALL* PFNJNICREATEJAVAVM)(JavaVM**, JNIEnv**, void*);

JNI_STRUCT_BEGIN(JNIEnv, 4)
    jint            JNIFUNC(GetVersion)                     (JNIEnv*);
    jclass          JNIFUNC(DefineClass)                    (JNIEnv*, const char*, jobject, const jbyte*, jint);
    jclass          JNIFUNC(FindClass)                      (JNIEnv*, const char*);
    jmethodID       JNIFUNC(FromReflectedMethod)            (JNIEnv*, jobject);
    jfieldID        JNIFUNC(FromReflectedField)             (JNIEnv*, jobject);
    jobject         JNIFUNC(ToReflectedMethod)              (JNIEnv*, jclass, jmethodID, jboolean);
    jclass          JNIFUNC(GetSuperclass)                  (JNIEnv*, jclass);
    jboolean        JNIFUNC(IsAssignableFrom)               (JNIEnv*, jclass, jclass);
    jobject         JNIFUNC(ToReflectedField)               (JNIEnv*, jclass, jfieldID, jboolean);
    jint            JNIFUNC(Throw)                          (JNIEnv*, jthrowable);
    jint            JNIFUNC(ThrowNew)                       (JNIEnv*, jclass, const char*);
    jthrowable      JNIFUNC(ExceptionOccurred)              (JNIEnv*);
    void            JNIFUNC(ExceptionDescribe)              (JNIEnv*);
    void            JNIFUNC(ExceptionClear)                 (JNIEnv*);
    void            JNIFUNC(FatalError)                     (JNIEnv*, const char*);
    jint            JNIFUNC(PushLocalFrame)                 (JNIEnv*, jint);
    jobject         JNIFUNC(PopLocalFrame)                  (JNIEnv*, jobject);
    jobject         JNIFUNC(NewGlobalRef)                   (JNIEnv*, jobject);
    void            JNIFUNC(DeleteGlobalRef)                (JNIEnv*, jobject);
    void            JNIFUNC(DeleteLocalRef)                 (JNIEnv*, jobject);
    jboolean        JNIFUNC(IsSameObject)                   (JNIEnv*, jobject, jobject);
    jobject         JNIFUNC(NewLocalRef)                    (JNIEnv*, jobject);
    jint            JNIFUNC(EnsureLocalCapacity)            (JNIEnv*, jint);
    jobject         JNIFUNC(AllocObject)                    (JNIEnv*, jclass);
    jobject         JNIFUNC(NewObject)                      (JNIEnv*, jclass, jmethodID, ...);
    jobject         JNIFUNC(NewObjectV)                     (JNIEnv*, jclass, jmethodID, va_list);
    jobject         JNIFUNC(NewObjectA)                     (JNIEnv*, jclass, jmethodID, const jvalue*);
    jclass          JNIFUNC(GetObjectClass)                 (JNIEnv*, jobject);
    jboolean        JNIFUNC(IsInstanceOf)                   (JNIEnv*, jobject, jclass);
    jmethodID       JNIFUNC(GetMethodID)                    (JNIEnv*, jclass, const char*, const char*);
    jobject         JNIFUNC(CallObjectMethod)               (JNIEnv*, jobject, jmethodID, ...);
    jobject         JNIFUNC(CallObjectMethodV)              (JNIEnv*, jobject, jmethodID, va_list);
    jobject         JNIFUNC(CallObjectMethodA)              (JNIEnv*, jobject, jmethodID, const jvalue*);
    jboolean        JNIFUNC(CallBooleanMethod)              (JNIEnv*, jobject, jmethodID, ...);
    jboolean        JNIFUNC(CallBooleanMethodV)             (JNIEnv*, jobject, jmethodID, va_list);
    jboolean        JNIFUNC(CallBooleanMethodA)             (JNIEnv*, jobject, jmethodID, const jvalue*);
    jbyte           JNIFUNC(CallByteMethod)                 (JNIEnv*, jobject, jmethodID, ...);
    jbyte           JNIFUNC(CallByteMethodV)                (JNIEnv*, jobject, jmethodID, va_list);
    jbyte           JNIFUNC(CallByteMethodA)                (JNIEnv*, jobject, jmethodID, const jvalue*);
    jchar           JNIFUNC(CallCharMethod)                 (JNIEnv*, jobject, jmethodID, ...);
    jchar           JNIFUNC(CallCharMethodV)                (JNIEnv*, jobject, jmethodID, va_list);
    jchar           JNIFUNC(CallCharMethodA)                (JNIEnv*, jobject, jmethodID, const jvalue*);
    jshort          JNIFUNC(CallShortMethod)                (JNIEnv*, jobject, jmethodID, ...);
    jshort          JNIFUNC(CallShortMethodV)               (JNIEnv*, jobject, jmethodID, va_list);
    jshort          JNIFUNC(CallShortMethodA)               (JNIEnv*, jobject, jmethodID, const jvalue*);
    jint            JNIFUNC(CallIntMethod)                  (JNIEnv*, jobject, jmethodID, ...);
    jint            JNIFUNC(CallIntMethodV)                 (JNIEnv*, jobject, jmethodID, va_list);
    jint            JNIFUNC(CallIntMethodA)                 (JNIEnv*, jobject, jmethodID, const jvalue*);
    jlong           JNIFUNC(CallLongMethod)                 (JNIEnv*, jobject, jmethodID, ...);
    jlong           JNIFUNC(CallLongMethodV)                (JNIEnv*, jobject, jmethodID, va_list);
    jlong           JNIFUNC(CallLongMethodA)                (JNIEnv*, jobject, jmethodID, const jvalue*);
    jfloat          JNIFUNC(CallFloatMethod)                (JNIEnv*, jobject, jmethodID, ...);
    jfloat          JNIFUNC(CallFloatMethodV)               (JNIEnv*, jobject, jmethodID, va_list);
    jfloat          JNIFUNC(CallFloatMethodA)               (JNIEnv*, jobject, jmethodID, const jvalue*);
    jdouble         JNIFUNC(CallDoubleMethod)               (JNIEnv*, jobject, jmethodID, ...);
    jdouble         JNIFUNC(CallDoubleMethodV)              (JNIEnv*, jobject, jmethodID, va_list);
    jdouble         JNIFUNC(CallDoubleMethodA)              (JNIEnv*, jobject, jmethodID, const jvalue*);
    void            JNIFUNC(CallVoidMethod)                 (JNIEnv*, jobject, jmethodID, ...);
    void            JNIFUNC(CallVoidMethodV)                (JNIEnv*, jobject, jmethodID, va_list);
    void            JNIFUNC(CallVoidMethodA)                (JNIEnv*, jobject, jmethodID, const jvalue*);
    jobject         JNIFUNC(CallNonvirtualObjectMethod)     (JNIEnv*, jobject, jclass, jmethodID, ...);
    jobject         JNIFUNC(CallNonvirtualObjectMethodV)    (JNIEnv*, jobject, jclass, jmethodID, va_list);
    jobject         JNIFUNC(CallNonvirtualObjectMethodA)    (JNIEnv*, jobject, jclass, jmethodID, const jvalue*);
    jboolean        JNIFUNC(CallNonvirtualBooleanMethod)    (JNIEnv*, jobject, jclass, jmethodID, ...);
    jboolean        JNIFUNC(CallNonvirtualBooleanMethodV)   (JNIEnv*, jobject, jclass, jmethodID, va_list);
    jboolean        JNIFUNC(CallNonvirtualBooleanMethodA)   (JNIEnv*, jobject, jclass, jmethodID, const jvalue*);
    jbyte           JNIFUNC(CallNonvirtualByteMethod)       (JNIEnv*, jobject, jclass, jmethodID, ...);
    jbyte           JNIFUNC(CallNonvirtualByteMethodV)      (JNIEnv*, jobject, jclass, jmethodID, va_list);
    jbyte           JNIFUNC(CallNonvirtualByteMethodA)      (JNIEnv*, jobject, jclass, jmethodID, const jvalue*);
    jchar           JNIFUNC(CallNonvirtualCharMethod)       (JNIEnv*, jobject, jclass, jmethodID, ...);
    jchar           JNIFUNC(CallNonvirtualCharMethodV)      (JNIEnv*, jobject, jclass, jmethodID, va_list);
    jchar           JNIFUNC(CallNonvirtualCharMethodA)      (JNIEnv*, jobject, jclass, jmethodID, const jvalue*);
    jshort          JNIFUNC(CallNonvirtualShortMethod)      (JNIEnv*, jobject, jclass, jmethodID, ...);
    jshort          JNIFUNC(CallNonvirtualShortMethodV)     (JNIEnv*, jobject, jclass, jmethodID, va_list);
    jshort          JNIFUNC(CallNonvirtualShortMethodA)     (JNIEnv*, jobject, jclass, jmethodID, const jvalue*);
    jint            JNIFUNC(CallNonvirtualIntMethod)        (JNIEnv*, jobject, jclass, jmethodID, ...);
    jint            JNIFUNC(CallNonvirtualIntMethodV)       (JNIEnv*, jobject, jclass, jmethodID, va_list);
    jint            JNIFUNC(CallNonvirtualIntMethodA)       (JNIEnv*, jobject, jclass, jmethodID, const jvalue*);
    jlong           JNIFUNC(CallNonvirtualLongMethod)       (JNIEnv*, jobject, jclass, jmethodID, ...);
    jlong           JNIFUNC(CallNonvirtualLongMethodV)      (JNIEnv*, jobject, jclass, jmethodID, va_list);
    jlong           JNIFUNC(CallNonvirtualLongMethodA)      (JNIEnv*, jobject, jclass, jmethodID, const jvalue*);
    jfloat          JNIFUNC(CallNonvirtualFloatMethod)      (JNIEnv*, jobject, jclass, jmethodID, ...);
    jfloat          JNIFUNC(CallNonvirtualFloatMethodV)     (JNIEnv*, jobject, jclass, jmethodID, va_list);
    jfloat          JNIFUNC(CallNonvirtualFloatMethodA)     (JNIEnv*, jobject, jclass, jmethodID, const jvalue*);
    jdouble         JNIFUNC(CallNonvirtualDoubleMethod)     (JNIEnv*, jobject, jclass, jmethodID, ...);
    jdouble         JNIFUNC(CallNonvirtualDoubleMethodV)    (JNIEnv*, jobject, jclass, jmethodID, va_list);
    jdouble         JNIFUNC(CallNonvirtualDoubleMethodA)    (JNIEnv*, jobject, jclass, jmethodID, const jvalue*);
    void            JNIFUNC(CallNonvirtualVoidMethod)       (JNIEnv*, jobject, jclass, jmethodID, ...);
    void            JNIFUNC(CallNonvirtualVoidMethodV)      (JNIEnv*, jobject, jclass, jmethodID, va_list);
    void            JNIFUNC(CallNonvirtualVoidMethodA)      (JNIEnv*, jobject, jclass, jmethodID, const jvalue*);
    jfieldID        JNIFUNC(GetFieldID)                     (JNIEnv*, jclass, const char*, const char*);
    jobject         JNIFUNC(GetObjectField)                 (JNIEnv*, jobject, jfieldID);
    jboolean        JNIFUNC(GetBooleanField)                (JNIEnv*, jobject, jfieldID);
    jbyte           JNIFUNC(GetByteField)                   (JNIEnv*, jobject, jfieldID);
    jchar           JNIFUNC(GetCharField)                   (JNIEnv*, jobject, jfieldID);
    jshort          JNIFUNC(GetShortField)                  (JNIEnv*, jobject, jfieldID);
    jint            JNIFUNC(GetIntField)                    (JNIEnv*, jobject, jfieldID);
    jlong           JNIFUNC(GetLongField)                   (JNIEnv*, jobject, jfieldID);
    jfloat          JNIFUNC(GetFloatField)                  (JNIEnv*, jobject, jfieldID);
    jdouble         JNIFUNC(GetDoubleField)                 (JNIEnv*, jobject, jfieldID);
    void            JNIFUNC(SetObjectField)                 (JNIEnv*, jobject, jfieldID, jobject);
    void            JNIFUNC(SetBooleanField)                (JNIEnv*, jobject, jfieldID, jboolean);
    void            JNIFUNC(SetByteField)                   (JNIEnv*, jobject, jfieldID, jbyte);
    void            JNIFUNC(SetCharField)                   (JNIEnv*, jobject, jfieldID, jchar);
    void            JNIFUNC(SetShortField)                  (JNIEnv*, jobject, jfieldID, jshort);
    void            JNIFUNC(SetIntField)                    (JNIEnv*, jobject, jfieldID, jint);
    void            JNIFUNC(SetLongField)                   (JNIEnv*, jobject, jfieldID, jlong);
    void            JNIFUNC(SetFloatField)                  (JNIEnv*, jobject, jfieldID, jfloat);
    void            JNIFUNC(SetDoubleField)                 (JNIEnv*, jobject, jfieldID, jdouble);
    jmethodID       JNIFUNC(GetStaticMethodID)              (JNIEnv*, jclass, const char*, const char*);
    jobject         JNIFUNC(CallStaticObjectMethod)         (JNIEnv*, jclass, jmethodID, ...);
    jobject         JNIFUNC(CallStaticObjectMethodV)        (JNIEnv*, jclass, jmethodID, va_list);
    jobject         JNIFUNC(CallStaticObjectMethodA)        (JNIEnv*, jclass, jmethodID, const jvalue*);
    jboolean        JNIFUNC(CallStaticBooleanMethod)        (JNIEnv*, jclass, jmethodID, ...);
    jboolean        JNIFUNC(CallStaticBooleanMethodV)       (JNIEnv*, jclass, jmethodID, va_list);
    jboolean        JNIFUNC(CallStaticBooleanMethodA)       (JNIEnv*, jclass, jmethodID, const jvalue*);
    jbyte           JNIFUNC(CallStaticByteMethod)           (JNIEnv*, jclass, jmethodID, ...);
    jbyte           JNIFUNC(CallStaticByteMethodV)          (JNIEnv*, jclass, jmethodID, va_list);
    jbyte           JNIFUNC(CallStaticByteMethodA)          (JNIEnv*, jclass, jmethodID, const jvalue*);
    jchar           JNIFUNC(CallStaticCharMethod)           (JNIEnv*, jclass, jmethodID, ...);
    jchar           JNIFUNC(CallStaticCharMethodV)          (JNIEnv*, jclass, jmethodID, va_list);
    jchar           JNIFUNC(CallStaticCharMethodA)          (JNIEnv*, jclass, jmethodID, const jvalue*);
    jshort          JNIFUNC(CallStaticShortMethod)          (JNIEnv*, jclass, jmethodID, ...);
    jshort          JNIFUNC(CallStaticShortMethodV)         (JNIEnv*, jclass, jmethodID, va_list);
    jshort          JNIFUNC(CallStaticShortMethodA)         (JNIEnv*, jclass, jmethodID, const jvalue*);
    jint            JNIFUNC(CallStaticIntMethod)            (JNIEnv*, jclass, jmethodID, ...);
    jint            JNIFUNC(CallStaticIntMethodV)           (JNIEnv*, jclass, jmethodID, va_list);
    jint            JNIFUNC(CallStaticIntMethodA)           (JNIEnv*, jclass, jmethodID, const jvalue*);
    jlong           JNIFUNC(CallStaticLongMethod)           (JNIEnv*, jclass, jmethodID, ...);
    jlong           JNIFUNC(CallStaticLongMethodV)          (JNIEnv*, jclass, jmethodID, va_list);
    jlong           JNIFUNC(CallStaticLongMethodA)          (JNIEnv*, jclass, jmethodID, const jvalue*);
    jfloat          JNIFUNC(CallStaticFloatMethod)          (JNIEnv*, jclass, jmethodID, ...);
    jfloat          JNIFUNC(CallStaticFloatMethodV)         (JNIEnv*, jclass, jmethodID, va_list);
    jfloat          JNIFUNC(CallStaticFloatMethodA)         (JNIEnv*, jclass, jmethodID, const jvalue*);
    jdouble         JNIFUNC(CallStaticDoubleMethod)         (JNIEnv*, jclass, jmethodID, ...);
    jdouble         JNIFUNC(CallStaticDoubleMethodV)        (JNIEnv*, jclass, jmethodID, va_list);
    jdouble         JNIFUNC(CallStaticDoubleMethodA)        (JNIEnv*, jclass, jmethodID, const jvalue*);
    void            JNIFUNC(CallStaticVoidMethod)           (JNIEnv*, jclass, jmethodID, ...);
    void            JNIFUNC(CallStaticVoidMethodV)          (JNIEnv*, jclass, jmethodID, va_list);
    void            JNIFUNC(CallStaticVoidMethodA)          (JNIEnv*, jclass, jmethodID, const jvalue*);
    jfieldID        JNIFUNC(GetStaticFieldID)               (JNIEnv*, jclass, const char*, const char*);
    jobject         JNIFUNC(GetStaticObjectField)           (JNIEnv*, jclass, jfieldID);
    jboolean        JNIFUNC(GetStaticBooleanField)          (JNIEnv*, jclass, jfieldID);
    jbyte           JNIFUNC(GetStaticByteField)             (JNIEnv*, jclass, jfieldID);
    jchar           JNIFUNC(GetStaticCharField)             (JNIEnv*, jclass, jfieldID);
    jshort          JNIFUNC(GetStaticShortField)            (JNIEnv*, jclass, jfieldID);
    jint            JNIFUNC(GetStaticIntField)              (JNIEnv*, jclass, jfieldID);
    jlong           JNIFUNC(GetStaticLongField)             (JNIEnv*, jclass, jfieldID);
    jfloat          JNIFUNC(GetStaticFloatField)            (JNIEnv*, jclass, jfieldID);
    jdouble         JNIFUNC(GetStaticDoubleField)           (JNIEnv*, jclass, jfieldID);
    void            JNIFUNC(SetStaticObjectField)           (JNIEnv*, jclass, jfieldID, jobject);
    void            JNIFUNC(SetStaticBooleanField)          (JNIEnv*, jclass, jfieldID, jboolean);
    void            JNIFUNC(SetStaticByteField)             (JNIEnv*, jclass, jfieldID, jbyte);
    void            JNIFUNC(SetStaticCharField)             (JNIEnv*, jclass, jfieldID, jchar);
    void            JNIFUNC(SetStaticShortField)            (JNIEnv*, jclass, jfieldID, jshort);
    void            JNIFUNC(SetStaticIntField)              (JNIEnv*, jclass, jfieldID, jint);
    void            JNIFUNC(SetStaticLongField)             (JNIEnv*, jclass, jfieldID, jlong);
    void            JNIFUNC(SetStaticFloatField)            (JNIEnv*, jclass, jfieldID, jfloat);
    void            JNIFUNC(SetStaticDoubleField)           (JNIEnv*, jclass, jfieldID, jdouble);
    jstring         JNIFUNC(NewString)                      (JNIEnv*, const jchar*, jint);
    jint            JNIFUNC(GetStringLength)                (JNIEnv*, jstring);
    const jchar*    JNIFUNC(GetStringChars)                 (JNIEnv*, jstring, jboolean*);
    void            JNIFUNC(ReleaseStringChars)             (JNIEnv*, jstring, const jchar*);
    jstring         JNIFUNC(NewStringUTF)                   (JNIEnv*, const char*);
    jint            JNIFUNC(GetStringUTFLength)             (JNIEnv*, jstring);
    const char*     JNIFUNC(GetStringUTFChars)              (JNIEnv*, jstring, jboolean*);
    void            JNIFUNC(ReleaseStringUTFChars)          (JNIEnv*, jstring, const char*);
    jint            JNIFUNC(GetArrayLength)                 (JNIEnv*, jarray);
    jobjectArray    JNIFUNC(NewObjectArray)                 (JNIEnv*, jint, jclass, jobject);
    jobject         JNIFUNC(GetObjectArrayElement)          (JNIEnv*, jobjectArray, jint);
    void            JNIFUNC(SetObjectArrayElement)          (JNIEnv*, jobjectArray, jint, jobject);
    jbooleanArray   JNIFUNC(NewBooleanArray)                (JNIEnv*, jint);
    jbyteArray      JNIFUNC(NewByteArray)                   (JNIEnv*, jint);
    jcharArray      JNIFUNC(NewCharArray)                   (JNIEnv*, jint);
    jshortArray     JNIFUNC(NewShortArray)                  (JNIEnv*, jint);
    jintArray       JNIFUNC(NewIntArray)                    (JNIEnv*, jint);
    jlongArray      JNIFUNC(NewLongArray)                   (JNIEnv*, jint);
    jfloatArray     JNIFUNC(NewFloatArray)                  (JNIEnv*, jint);
    jdoubleArray    JNIFUNC(NewDoubleArray)                 (JNIEnv*, jint);
    jboolean*       JNIFUNC(GetBooleanArrayElements)        (JNIEnv*, jbooleanArray, jboolean*);
    jbyte*          JNIFUNC(GetByteArrayElements)           (JNIEnv*, jbyteArray, jboolean*);
    jchar*          JNIFUNC(GetCharArrayElements)           (JNIEnv*, jcharArray, jboolean*);
    jshort*         JNIFUNC(GetShortArrayElements)          (JNIEnv*, jshortArray, jboolean*);
    jint*           JNIFUNC(GetIntArrayElements)            (JNIEnv*, jintArray, jboolean*);
    jlong*          JNIFUNC(GetLongArrayElements)           (JNIEnv*, jlongArray, jboolean*);
    jfloat*         JNIFUNC(GetFloatArrayElements)          (JNIEnv*, jfloatArray, jboolean*);
    jdouble*        JNIFUNC(GetDoubleArrayElements)         (JNIEnv*, jdoubleArray, jboolean*);
    void            JNIFUNC(ReleaseBooleanArrayElements)    (JNIEnv*, jbooleanArray, jboolean*, jint);
    void            JNIFUNC(ReleaseByteArrayElements)       (JNIEnv*, jbyteArray, jbyte*, jint);
    void            JNIFUNC(ReleaseCharArrayElements)       (JNIEnv*, jcharArray, jchar*, jint);
    void            JNIFUNC(ReleaseShortArrayElements)      (JNIEnv*, jshortArray, jshort*, jint);
    void            JNIFUNC(ReleaseIntArrayElements)        (JNIEnv*, jintArray, jint*, jint);
    void            JNIFUNC(ReleaseLongArrayElements)       (JNIEnv*, jlongArray, jlong*, jint);
    void            JNIFUNC(ReleaseFloatArrayElements)      (JNIEnv*, jfloatArray, jfloat*, jint);
    void            JNIFUNC(ReleaseDoubleArrayElements)     (JNIEnv*, jdoubleArray, jdouble*, jint);
    void            JNIFUNC(GetBooleanArrayRegion)          (JNIEnv*, jbooleanArray, jint, jint l, jboolean*);
    void            JNIFUNC(GetByteArrayRegion)             (JNIEnv*, jbyteArray, jint, jint, jbyte*);
    void            JNIFUNC(GetCharArrayRegion)             (JNIEnv*, jcharArray, jint, jint, jchar*);
    void            JNIFUNC(GetShortArrayRegion)            (JNIEnv*, jshortArray, jint, jint, jshort*);
    void            JNIFUNC(GetIntArrayRegion)              (JNIEnv*, jintArray, jint, jint, jint*);
    void            JNIFUNC(GetLongArrayRegion)             (JNIEnv*, jlongArray, jint, jint, jlong*);
    void            JNIFUNC(GetFloatArrayRegion)            (JNIEnv*, jfloatArray, jint, jint, jfloat*);
    void            JNIFUNC(GetDoubleArrayRegion)           (JNIEnv*, jdoubleArray, jint, jint, jdouble*);
    void            JNIFUNC(SetBooleanArrayRegion)          (JNIEnv*, jbooleanArray, jint, jint l, const jboolean*);
    void            JNIFUNC(SetByteArrayRegion)             (JNIEnv*, jbyteArray, jint, jint, const jbyte*);
    void            JNIFUNC(SetCharArrayRegion)             (JNIEnv*, jcharArray, jint, jint, const jchar*);
    void            JNIFUNC(SetShortArrayRegion)            (JNIEnv*, jshortArray, jint, jint, const jshort*);
    void            JNIFUNC(SetIntArrayRegion)              (JNIEnv*, jintArray, jint, jint, const jint*);
    void            JNIFUNC(SetLongArrayRegion)             (JNIEnv*, jlongArray, jint, jint, const jlong*);
    void            JNIFUNC(SetFloatArrayRegion)            (JNIEnv*, jfloatArray, jint, jint, const jfloat*);
    void            JNIFUNC(SetDoubleArrayRegion)           (JNIEnv*, jdoubleArray, jint, jint, const jdouble*);
    jint            JNIFUNC(RegisterNatives)                (JNIEnv*, jclass, const JNINativeMethod*, jint);
    jint            JNIFUNC(UnregisterNatives)              (JNIEnv*, jclass);
    jint            JNIFUNC(MonitorEnter)                   (JNIEnv*, jobject);
    jint            JNIFUNC(MonitorExit)                    (JNIEnv*, jobject);
    jint            JNIFUNC(GetJavaVM)                      (JNIEnv*, JavaVM**);
    void            JNIFUNC(GetStringRegion)                (JNIEnv*, jstring, jint, jint, jchar*);
    void            JNIFUNC(GetStringUTFRegion)             (JNIEnv*, jstring, jint, jint, char*);
    void*           JNIFUNC(GetPrimitiveArrayCritical)      (JNIEnv*, jarray, jboolean*);
    void            JNIFUNC(ReleasePrimitiveArrayCritical)  (JNIEnv*, jarray, void*, jint);
    const jchar*    JNIFUNC(GetStringCritical)              (JNIEnv*, jstring, jboolean*);
    void            JNIFUNC(ReleaseStringCritical)          (JNIEnv*, jstring, const jchar*);
    jweak           JNIFUNC(NewWeakGlobalRef)               (JNIEnv*, jobject);
    void            JNIFUNC(DeleteWeakGlobalRef)            (JNIEnv*, jweak);
    jboolean        JNIFUNC(ExceptionCheck)                 (JNIEnv*);
    jobject         JNIFUNC(NewDirectByteBuffer)            (JNIEnv*, void*, jlong);
    void*           JNIFUNC(GetDirectBufferAddress)         (JNIEnv*, jobject);
    jlong           JNIFUNC(GetDirectBufferCapacity)        (JNIEnv*, jobject);
JNI_STRUCT_END()

JNI_STRUCT_BEGIN(JavaVM, 3)
    jint            JNIFUNC(DestroyJavaVM)                  (JavaVM*);
    jint            JNIFUNC(AttachCurrentThread)            (JavaVM*, void**, void*);
    jint            JNIFUNC(DetachCurrentThread)            (JavaVM*);
    jint            JNIFUNC(GetEnv)                         (JavaVM*, void**, jint);
    jint            JNIFUNC(AttachCurrentThreadAsDaemon)    (JavaVM*, void**, void*);
JNI_STRUCT_END()

#endif
