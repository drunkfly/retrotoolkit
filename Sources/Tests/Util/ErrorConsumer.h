#ifndef TESTS_UTIL_ERRORCONSUMER_H
#define TESTS_UTIL_ERRORCONSUMER_H

#include "Compiler/CompilerError.h"
#include <memory>

class ErrorConsumer
{
public:
    ErrorConsumer();
    ~ErrorConsumer();

    void setError(const CompilerError& error);
    std::string errorMessage() const;

private:
    std::unique_ptr<CompilerError> mError;

    DISABLE_COPY(ErrorConsumer);
};

#endif
