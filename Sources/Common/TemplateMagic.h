#ifndef COMMON_UINT8ARRAY_H
#define COMMON_UINT8ARRAY_H

#include <utility>
#include <array>
#include <stddef.h>
#include <stdint.h>

template <typename T, size_t N> struct ArrayType { enum { Size = N }; };
template <typename T, size_t N> constexpr ArrayType<T, N> arrayType(const T (&array)[N]) { return ArrayType<T, N>(); }

template <typename T, size_t N, size_t... I>
    constexpr std::array<uint8_t, N> toArrayImpl(T (&&array)[N], std::index_sequence<I...>)
{
    return { {uint8_t(array[I])...} };
}

template <typename T, size_t N> constexpr std::array<uint8_t, N> toArray(T (&&array)[N])
{
    return toArrayImpl(std::move(array), std::make_index_sequence<N>{});
}

#endif
