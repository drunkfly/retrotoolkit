#ifndef TESTS_UTIL_TESTUTIL_H
#define TESTS_UTIL_TESTUTIL_H

#include "Common/GC.h"
#include "Compiler/Linker/CompiledOutput.h"
#include "Tests/Util/DataBlob.h"
#include "Tests/Util/ErrorConsumer.h"

DataBlob assemble(ErrorConsumer& errorConsumer, const char* source);

#endif
