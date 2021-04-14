#ifndef COMMON_UINT8ARRAY_H
#define COMMON_UINT8ARRAY_H

#include "Common/Common.h"

template <typename T, size_t N> struct ArrayType { enum { Size = N }; };
template <typename T, size_t N> constexpr ArrayType<T, N> arrayType(const T (&array)[N]) { return ArrayType<T, N>(); }

template <typename T, size_t N, size_t... I>
    constexpr std::array<uint8_t, N> toUInt8ArrayImpl(T (&&array)[N], std::index_sequence<I...>)
{
    return { {uint8_t(array[I])...} };
}

template <typename T, size_t N, size_t... I>
    constexpr std::array<uint16_t, N> toUInt16ArrayImpl(T (&&array)[N], std::index_sequence<I...>)
{
    return { {uint16_t(array[I])...} };
}

template <typename T, size_t N> constexpr std::array<uint8_t, N> toUInt8Array(T (&&array)[N])
{
    return toUInt8ArrayImpl(std::move(array), std::make_index_sequence<N>{});
}

template <typename T, size_t N> constexpr std::array<uint16_t, N> toUInt16Array(T (&&array)[N])
{
    return toUInt16ArrayImpl(std::move(array), std::make_index_sequence<N>{});
}

#endif
