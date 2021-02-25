#ifndef COMMON_TEMPLATEMAGIC_H
#define COMMON_TEMPLATEMAGIC_H

#include <stddef.h>
#include <stdint.h>

template <size_t N> class ByteArray
{
public:
    explicit ByteArray(const uint8_t* ptr) : mPtr(ptr) {}
    size_t size() const { return N; }
    const uint8_t* data() const { return mPtr; }
private:
    const uint8_t* mPtr;
};

template <size_t N> constexpr size_t sizeofByteArray(const uint8_t (&array)[N]) { return N; }
template <size_t N> constexpr ByteArray<N> makeByteArray(const uint8_t(&array)[N]) { return ByteArray<N>(array); }

#endif
