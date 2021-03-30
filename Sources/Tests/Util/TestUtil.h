#ifndef TESTS_UTIL_TESTUTIL_H
#define TESTS_UTIL_TESTUTIL_H

#include "Tests/Common.h"
#include "Common/GC.h"
#include "Compiler/Linker/CompiledOutput.h"
#include "Tests/Util/DataBlob.h"
#include "Tests/Util/ErrorConsumer.h"

DataBlob assemble(ErrorConsumer& errorConsumer, const char* source);
DataBlob assemble2(ErrorConsumer& errorConsumer, const char* source1, const char* source2);

#endif
