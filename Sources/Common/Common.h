#ifndef COMMON_COMMON_H
#define COMMON_COMMON_H

#define DISABLE_COPY(NAME) \
    NAME(const NAME&) = delete; \
    NAME& operator=(const NAME&) = delete

#endif
