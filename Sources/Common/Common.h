#ifndef COMMON_COMMON_H
#define COMMON_COMMON_H

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <locale>
#include <codecvt>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <utility>
#include <memory>
#include <optional>
#include <new>
#include <vector>
#include <algorithm>
#include <map>
#include <iomanip>
#include <mutex>

#define DISABLE_COPY(NAME) \
    NAME(const NAME&) = delete; \
    NAME& operator=(const NAME&) = delete

#endif
