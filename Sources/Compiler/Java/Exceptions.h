#ifndef COMPILER_JAVA_EXCEPTIONS_H
#define COMPILER_JAVA_EXCEPTIONS_H

#include "Compiler/CompilerError.h"

#define JTRY \
    try {
#define JCATCH \
    } catch (const CompilerError& e) { javaThrow(e); \
    } catch (const std::bad_alloc& e) { javaThrow(e); \
    } catch (const std::exception& e) { javaThrow(e); \
    } catch (...) { javaThrow(); }

class JNIClassRef;

void javaThrow();
void javaThrow(const std::bad_alloc& e);
void javaThrow(const std::exception& e);
void javaThrow(const CompilerError& e);
void javaThrow(const JNIClassRef& exceptionClass, const char* prefix, const char* message);

#endif
