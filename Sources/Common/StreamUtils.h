#ifndef COMMON_STREAMUTILS_H
#define COMMON_STREAMUTILS_H

#include <sstream>
#include <stdint.h>

void writeByte(std::stringstream& ss, uint8_t byte);
void writeWordLE(std::stringstream& ss, uint16_t byte);

#endif
