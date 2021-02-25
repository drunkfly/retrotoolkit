#include "StreamUtils.h"

void writeByte(std::stringstream& ss, uint8_t byte)
{
    ss << static_cast<unsigned char>(byte);
}

void writeWordLE(std::stringstream& ss, uint16_t byte)
{
    ss << static_cast<unsigned char>(byte & 0xff);
    ss << static_cast<unsigned char>((byte >> 8) & 0xff);
}
