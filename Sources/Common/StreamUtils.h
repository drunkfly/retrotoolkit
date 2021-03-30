#ifndef COMMON_STREAMUTILS_H
#define COMMON_STREAMUTILS_H

#include "Common/Common.h"

void writeByte(std::stringstream& ss, uint8_t byte);
void writeWordLE(std::stringstream& ss, uint16_t byte);

#endif
