#ifndef COMMON_TEMPLATEMAGIC_H
#define COMMON_TEMPLATEMAGIC_H

#include <stddef.h>
#include <stdint.h>

template <size_t N> constexpr size_t sizeofByteArray(const uint8_t (&array)[N]) { return N; }

#endif
